#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "image_transfer.h"

static long file_size_of(FILE *fp)
{
    long cur;
    long size;

    if (fp == NULL)
    {
        return -1;
    }

    cur = ftell(fp);
    if (cur < 0)
    {
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        return -1;
    }

    size = ftell(fp);

    if (fseek(fp, cur, SEEK_SET) != 0)
    {
        return -1;
    }

    return size;
}

int sender_init(ImageSendContext *ctx, const char *image_path, uint32_t image_seq)
{
    long image_size;

    if (ctx == NULL || image_path == NULL)
    {
        return IMG_ERR_ARG;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->image_fp = fopen(image_path, "rb");
    if (ctx->image_fp == NULL)
    {
        return IMG_ERR_IO;
    }

    image_size = file_size_of(ctx->image_fp);
    if (image_size <= 0 || image_size > (long)IMAGE_MAX_SIZE)
    {
        fclose(ctx->image_fp);
        ctx->image_fp = NULL;
        return IMG_ERR_RANGE;
    }

    ctx->task.image_seq = image_seq;
    snprintf(ctx->task.image_path, sizeof(ctx->task.image_path), "%s", image_path);

    ctx->task.image_size = (uint32_t)image_size;
    ctx->task.chunk_size = IMAGE_CHUNK_DATA_SIZE;
    ctx->task.total_chunks = image_calc_total_chunks(ctx->task.image_size);
    ctx->task.current_cycle = 0;
    ctx->task.next_new_chunk = 0;
    ctx->task.state = IMG_STATE_READY;

    return IMG_OK;
}

void sender_close(ImageSendContext *ctx)
{
    if (ctx != NULL && ctx->image_fp != NULL)
    {
        fclose(ctx->image_fp);
        ctx->image_fp = NULL;
    }
}

int sender_build_chunk(ImageSendContext *ctx,
                       uint16_t chunk_index,
                       uint8_t flags,
                       ImageChunkPacket *pkt)
{
    long offset;
    size_t n;

    if (ctx == NULL || pkt == NULL || ctx->image_fp == NULL)
    {
        return IMG_ERR_ARG;
    }

    if (chunk_index >= ctx->task.total_chunks)
    {
        return IMG_ERR_RANGE;
    }

    memset(pkt, 0, sizeof(*pkt));

    offset = (long)chunk_index * (long)IMAGE_CHUNK_DATA_SIZE;
    if (fseek(ctx->image_fp, offset, SEEK_SET) != 0)
    {
        return IMG_ERR_IO;
    }

    n = fread(pkt->data, 1, IMAGE_CHUNK_DATA_SIZE, ctx->image_fp);
    if (n == 0 && ferror(ctx->image_fp))
    {
        return IMG_ERR_IO;
    }

    pkt->magic = IMG_MAGIC_CHUNK;
    pkt->image_seq = ctx->task.image_seq;
    pkt->chunk_index = chunk_index;
    pkt->total_chunks = ctx->task.total_chunks;
    pkt->image_size = ctx->task.image_size;
    pkt->data_len = (uint16_t)n;
    pkt->cycle_index = ctx->task.current_cycle;
    pkt->flags = flags;

    if (chunk_index == ctx->task.total_chunks - 1u)
    {
        pkt->flags |= IMG_FLAG_LAST_CHUNK;
    }

    pkt->crc16 = img_crc16_ccitt((const uint8_t *)pkt,
                                 sizeof(ImageChunkPacket) - sizeof(pkt->crc16));

    return IMG_OK;
}

int sender_parse_ack(ImageSendContext *ctx, const ImageFullAckPacket *ack)
{
    uint16_t crc;

    if (ctx == NULL || ack == NULL)
    {
        return IMG_ERR_ARG;
    }

    if (ack->magic != IMG_MAGIC_ACK || ack->image_seq != ctx->task.image_seq)
    {
        return IMG_ERR_ARG;
    }

    if (ack->bitmap_len > IMAGE_FULL_BITMAP_SIZE ||
        ack->total_chunks != ctx->task.total_chunks)
    {
        return IMG_ERR_RANGE;
    }

    crc = img_crc16_ccitt((const uint8_t *)ack,
                          sizeof(ImageFullAckPacket) - sizeof(ack->crc16));
    if (crc != ack->crc16)
    {
        return IMG_ERR_CRC;
    }

    memset(ctx->remote_recv_bitmap, 0, sizeof(ctx->remote_recv_bitmap));
    memcpy(ctx->remote_recv_bitmap, ack->bitmap, ack->bitmap_len);

    ctx->task.remote_received_chunks = ack->received_chunks;
    ctx->ack_received = 1;
    ctx->ack_recv_count++;

    if (ack->image_complete ||
        ack->received_chunks == ctx->task.total_chunks)
    {
        ctx->task.state = IMG_STATE_ALL_DONE;
    }

    return IMG_OK;
}

void sender_build_resend_list(ImageSendContext *ctx)
{
    uint16_t i;

    if (ctx == NULL)
    {
        return;
    }

    ctx->resend_count = 0;
    ctx->resend_pos = 0;

    for (i = 0; i < ctx->task.total_chunks; i++)
    {
        if (bitmap_is_set(ctx->local_sent_bitmap, i) &&
            !bitmap_is_set(ctx->remote_recv_bitmap, i))
        {
            ctx->resend_list[ctx->resend_count++] = i;
        }
    }
}

/*
 * Return:
 *   IMG_OK: pkt is valid and should be sent.
 *   IMG_ERR_RANGE: no more packet can be sent in this cycle.
 */
int sender_get_next_packet_for_cycle(ImageSendContext *ctx, ImageChunkPacket *pkt)
{
    uint16_t chunk_index;
    uint8_t flags;

    if (ctx == NULL || pkt == NULL)
    {
        return IMG_ERR_ARG;
    }

    if (ctx->task.state == IMG_STATE_ALL_DONE)
    {
        return IMG_ERR_RANGE;
    }

    /* 1. priority: resend missing chunks */
    if (ctx->resend_pos < ctx->resend_count)
    {
        chunk_index = ctx->resend_list[ctx->resend_pos++];
        flags = IMG_FLAG_RESEND;
    }
    /* 2. then send new chunks */
    else if (ctx->task.next_new_chunk < ctx->task.total_chunks)
    {
        chunk_index = ctx->task.next_new_chunk++;
        flags = IMG_FLAG_NORMAL;
        bitmap_set(ctx->local_sent_bitmap, chunk_index);
    }
    else
    {
        return IMG_ERR_RANGE;
    }

    if (sender_build_chunk(ctx, chunk_index, flags, pkt) != IMG_OK)
    {
        return IMG_ERR_IO;
    }

    ctx->task.sent_chunks++;
    return IMG_OK;
}

int sender_is_done(const ImageSendContext *ctx)
{
    return ctx != NULL && ctx->task.state == IMG_STATE_ALL_DONE;
}

/*
 * Demo mode:
 *   This does not implement real acoustic communication.
 *   It writes one cycle of packets into a binary file.
 *
 * Usage:
 *   ./sender_demo input.jpg tx_cycle_0.bin [ack_from_receiver.bin]
 *
 * If an ACK file is given, sender parses it first and then generates resend list.
 */
#ifdef IMAGE_SENDER_DEMO
int main(int argc, char **argv)
{
    ImageSendContext ctx;
    ImageChunkPacket pkt;
    FILE *out;
    int ret;
    uint16_t tx_count = 0;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <input_image> <tx_output_bin> [ack_bin]\n", argv[0]);
        return 1;
    }

    ret = sender_init(&ctx, argv[1], 1u);
    if (ret != IMG_OK)
    {
        fprintf(stderr, "sender_init failed: %d\n", ret);
        return 1;
    }

    if (argc >= 4)
    {
        ImageFullAckPacket ack;
        FILE *afp = fopen(argv[3], "rb");
        if (afp != NULL)
        {
            if (fread(&ack, 1, sizeof(ack), afp) == sizeof(ack))
            {
                ret = sender_parse_ack(&ctx, &ack);
                if (ret == IMG_OK)
                {
                    sender_build_resend_list(&ctx);
                    printf("ACK parsed. remote_received=%u, resend_count=%u\n",
                           ctx.task.remote_received_chunks, ctx.resend_count);
                }
                else
                {
                    printf("ACK parse failed: %d\n", ret);
                }
            }
            fclose(afp);
        }
    }

    out = fopen(argv[2], "wb");
    if (out == NULL)
    {
        fprintf(stderr, "open output failed\n");
        sender_close(&ctx);
        return 1;
    }

    while (tx_count < IMAGE_MAX_DATA_PER_CYCLE)
    {
        ret = sender_get_next_packet_for_cycle(&ctx, &pkt);
        if (ret != IMG_OK)
        {
            break;
        }

        fwrite(&pkt, 1, sizeof(pkt), out);
        tx_count++;
    }

    fclose(out);

    printf("image_size=%u, total_chunks=%u, packets_written=%u, output=%s\n",
           ctx.task.image_size, ctx.task.total_chunks, tx_count, argv[2]);

    sender_close(&ctx);
    return 0;
}
#endif
