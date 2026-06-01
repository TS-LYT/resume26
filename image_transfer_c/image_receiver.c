#include <stdio.h>
#include <stdint.h>
#include <string.h>
#if defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#endif
#include "image_transfer.h"

int receiver_init(ImageRecvContext *ctx,
                  const char *save_path,
                  uint32_t image_seq,
                  uint32_t image_size,
                  uint16_t total_chunks)
{
    if (ctx == NULL || save_path == NULL)
    {
        return IMG_ERR_ARG;
    }

    if (image_size == 0 || image_size > IMAGE_MAX_SIZE ||
        total_chunks == 0 || total_chunks > IMAGE_MAX_CHUNKS)
    {
        return IMG_ERR_RANGE;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->image_seq = image_seq;
    snprintf(ctx->save_path, sizeof(ctx->save_path), "%s", save_path);

    ctx->image_size = image_size;
    ctx->chunk_size = IMAGE_CHUNK_DATA_SIZE;
    ctx->total_chunks = total_chunks;

    /*
     * Use wb+ so we can seek and write by chunk index.
     * On embedded Linux, you can replace this with open/lseek/write.
     */
    ctx->image_fp = fopen(save_path, "wb+");
    if (ctx->image_fp == NULL)
    {
        return IMG_ERR_IO;
    }

    return IMG_OK;
}

void receiver_close(ImageRecvContext *ctx)
{
    if (ctx != NULL && ctx->image_fp != NULL)
    {
        fflush(ctx->image_fp);
        fclose(ctx->image_fp);
        ctx->image_fp = NULL;
    }
}

int receiver_handle_chunk(ImageRecvContext *ctx, const ImageChunkPacket *pkt)
{
    uint16_t crc;
    long offset;
    size_t n;

    if (ctx == NULL || pkt == NULL || ctx->image_fp == NULL)
    {
        return IMG_ERR_ARG;
    }

    if (pkt->magic != IMG_MAGIC_CHUNK ||
        pkt->image_seq != ctx->image_seq ||
        pkt->total_chunks != ctx->total_chunks ||
        pkt->data_len > IMAGE_CHUNK_DATA_SIZE ||
        pkt->chunk_index >= ctx->total_chunks)
    {
        return IMG_ERR_RANGE;
    }

    crc = img_crc16_ccitt((const uint8_t *)pkt,
                          sizeof(ImageChunkPacket) - sizeof(pkt->crc16));
    if (crc != pkt->crc16)
    {
        return IMG_ERR_CRC;
    }

    offset = (long)pkt->chunk_index * (long)IMAGE_CHUNK_DATA_SIZE;
    if (fseek(ctx->image_fp, offset, SEEK_SET) != 0)
    {
        return IMG_ERR_IO;
    }

    n = fwrite(pkt->data, 1, pkt->data_len, ctx->image_fp);
    if (n != pkt->data_len)
    {
        return IMG_ERR_IO;
    }

    /*
     * Count only once. Duplicated resend packets should not increase count.
     */
    if (!bitmap_is_set(ctx->recv_bitmap, pkt->chunk_index))
    {
        bitmap_set(ctx->recv_bitmap, pkt->chunk_index);
        ctx->received_chunks++;
    }

    if (ctx->received_chunks == ctx->total_chunks)
    {
        ctx->image_complete = 1;

    }

    return IMG_OK;
}

int receiver_build_ack(const ImageRecvContext *ctx,
                       uint16_t cycle_index,
                       uint8_t ack_seq,
                       ImageFullAckPacket *ack)
{
    if (ctx == NULL || ack == NULL)
    {
        return IMG_ERR_ARG;
    }

    memset(ack, 0, sizeof(*ack));

    ack->magic = IMG_MAGIC_ACK;
    ack->image_seq = ctx->image_seq;
    ack->total_chunks = ctx->total_chunks;
    ack->received_chunks = ctx->received_chunks;
    ack->lost_chunks = ctx->total_chunks - ctx->received_chunks;
    ack->cycle_index = cycle_index;
    ack->ack_seq = ack_seq;
    ack->image_complete = ctx->image_complete ? 1u : 0u;
    ack->bitmap_len = (uint16_t)((ctx->total_chunks + 7u) / 8u);

    memcpy(ack->bitmap, ctx->recv_bitmap, ack->bitmap_len);

    ack->crc16 = img_crc16_ccitt((const uint8_t *)ack,
                                 sizeof(ImageFullAckPacket) - sizeof(ack->crc16));

    return IMG_OK;
}

int receiver_is_done(const ImageRecvContext *ctx)
{
    return ctx != NULL && ctx->image_complete != 0;
}

/*
 * Demo mode:
 *   Read packets from a binary tx file, write output image, then generate 3 ACK files.
 *
 * Usage:
 *   ./receiver_demo tx_cycle_0.bin output.jpg ack_prefix
 *
 * Output:
 *   ack_prefix_0.bin
 *   ack_prefix_1.bin
 *   ack_prefix_2.bin
 */
#ifdef IMAGE_RECEIVER_DEMO
int main(int argc, char **argv)
{
    FILE *in;
    ImageChunkPacket pkt;
    ImageRecvContext ctx;
    int initialized = 0;
    uint32_t ok_count = 0;
    uint32_t bad_count = 0;

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <tx_input_bin> <output_image> <ack_prefix>\n", argv[0]);
        return 1;
    }

    in = fopen(argv[1], "rb");
    if (in == NULL)
    {
        fprintf(stderr, "open tx input failed\n");
        return 1;
    }

    while (fread(&pkt, 1, sizeof(pkt), in) == sizeof(pkt))
    {
        int ret;

        if (!initialized)
        {
            ret = receiver_init(&ctx,
                                argv[2],
                                pkt.image_seq,
                                pkt.image_size,
                                pkt.total_chunks);
            if (ret != IMG_OK)
            {
                fprintf(stderr, "receiver_init failed: %d\n", ret);
                fclose(in);
                return 1;
            }
            initialized = 1;
        }

        ret = receiver_handle_chunk(&ctx, &pkt);
        if (ret == IMG_OK)
        {
            ok_count++;
        }
        else
        {
            bad_count++;
        }
    }

    fclose(in);

    if (!initialized)
    {
        fprintf(stderr, "no packet received\n");
        return 1;
    }

    for (uint8_t i = 0; i < IMAGE_ACK_REPEAT_COUNT; i++)
    {
        char ack_name[512];
        ImageFullAckPacket ack;
        FILE *afp;

        snprintf(ack_name, sizeof(ack_name), "%s_%u.bin", argv[3], (unsigned)i);

        if (receiver_build_ack(&ctx, 0, i, &ack) == IMG_OK)
        {
            afp = fopen(ack_name, "wb");
            if (afp != NULL)
            {
                fwrite(&ack, 1, sizeof(ack), afp);
                fclose(afp);
            }
        }
    }

    printf("received_ok=%u, bad=%u, received_chunks=%u/%u, complete=%u\n",
           ok_count, bad_count, ctx.received_chunks, ctx.total_chunks, ctx.image_complete);

    receiver_close(&ctx);
    return 0;
}
#endif
