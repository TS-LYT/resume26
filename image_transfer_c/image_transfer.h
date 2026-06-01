#ifndef IMAGE_TRANSFER_H
#define IMAGE_TRANSFER_H

/*
 * Image transfer protocol for low-rate half-duplex acoustic link.
 *
 * Constraints:
 *   - image size <= 1MB
 *   - one transport frame can carry up to 640 bytes
 *   - one image chunk carries 400 bytes image data
 *   - one-way communication time is about 8 seconds
 *   - one cycle is 30 minutes
 *   - receiver sends 3 ACK packets at the end of each cycle
 *
 * Bitmap rule:
 *   bit = 1: chunk received
 *   bit = 0: chunk not received
 */

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_MAX_SIZE              (1024u * 1024u)
#define COMM_FRAME_MAX_SIZE         640u

#define IMAGE_CHUNK_DATA_SIZE       400u
#define IMAGE_MAX_CHUNKS            ((IMAGE_MAX_SIZE + IMAGE_CHUNK_DATA_SIZE - 1u) / IMAGE_CHUNK_DATA_SIZE)
/* 1MB / 400B => 2622 chunks */

#define IMAGE_FULL_BITMAP_SIZE      ((IMAGE_MAX_CHUNKS + 7u) / 8u)
/* 2622 bits => 328 bytes */

#define IMAGE_CYCLE_SEC             1800u
#define IMAGE_ONEWAY_SEC            8u
#define IMAGE_MAX_TX_PER_CYCLE      (IMAGE_CYCLE_SEC / IMAGE_ONEWAY_SEC) /* 225 */
#define IMAGE_ACK_REPEAT_COUNT      3u
#define IMAGE_MAX_DATA_PER_CYCLE    210u

#define IMAGE_PATH_LEN              256u

#define IMG_MAGIC_CHUNK             0x494D4743u /* "IMGC" */
#define IMG_MAGIC_ACK               0x494D4741u /* "IMGA" */

#define IMG_FLAG_NORMAL             0x00u
#define IMG_FLAG_RESEND             0x01u
#define IMG_FLAG_LAST_CHUNK         0x02u

#define IMG_OK                      0
#define IMG_ERR                    -1
#define IMG_ERR_ARG                -2
#define IMG_ERR_IO                 -3
#define IMG_ERR_CRC                -4
#define IMG_ERR_RANGE              -5

#pragma pack(push, 1)

typedef struct
{
    uint32_t magic;                         /* IMG_MAGIC_CHUNK */
    uint32_t image_seq;                     /* unique image id */
    uint16_t chunk_index;                   /* 0 ~ total_chunks-1 */
    uint16_t total_chunks;                  /* max 2622 */
    uint32_t image_size;                    /* <= 1MB */
    uint16_t data_len;                      /* <= 400 */
    uint16_t cycle_index;                   /* current cycle */
    uint8_t  flags;                         /* normal/resend/last */
    uint8_t  reserved;
    uint8_t  data[IMAGE_CHUNK_DATA_SIZE];
    uint16_t crc16;                         /* CRC for all fields before crc16 */
} ImageChunkPacket;

typedef struct
{
    uint32_t magic;                         /* IMG_MAGIC_ACK */
    uint32_t image_seq;
    uint16_t total_chunks;
    uint16_t received_chunks;
    uint16_t lost_chunks;
    uint16_t cycle_index;
    uint8_t  ack_seq;                       /* 0, 1, 2 */
    uint8_t  image_complete;                /* 1 if whole image received */
    uint16_t bitmap_len;                    /* <= IMAGE_FULL_BITMAP_SIZE */
    uint8_t  bitmap[IMAGE_FULL_BITMAP_SIZE];
    uint16_t crc16;
} ImageFullAckPacket;

#pragma pack(pop)

typedef enum
{
    IMG_STATE_IDLE = 0,
    IMG_STATE_READY,
    IMG_STATE_CYCLE_SEND,
    IMG_STATE_WAIT_ACK,
    IMG_STATE_PARSE_ACK,
    IMG_STATE_ALL_DONE,
    IMG_STATE_FAILED
} ImageTransState;

typedef struct
{
    uint32_t image_seq;
    char image_path[IMAGE_PATH_LEN];

    uint32_t image_size;
    uint16_t chunk_size;
    uint16_t total_chunks;

    uint16_t current_cycle;
    uint16_t next_new_chunk;

    uint16_t sent_chunks;
    uint16_t remote_received_chunks;

    ImageTransState state;
} ImageTransferTask;

typedef struct
{
    ImageTransferTask task;
    FILE *image_fp;

    uint8_t local_sent_bitmap[IMAGE_FULL_BITMAP_SIZE];
    uint8_t remote_recv_bitmap[IMAGE_FULL_BITMAP_SIZE];

    uint16_t resend_list[IMAGE_MAX_CHUNKS];
    uint16_t resend_count;
    uint16_t resend_pos;

    uint8_t ack_received;
    uint8_t ack_recv_count;
} ImageSendContext;

typedef struct
{
    uint32_t image_seq;
    char save_path[IMAGE_PATH_LEN];
    FILE *image_fp;

    uint32_t image_size;
    uint16_t chunk_size;
    uint16_t total_chunks;

    uint16_t received_chunks;
    uint8_t recv_bitmap[IMAGE_FULL_BITMAP_SIZE];

    uint8_t image_complete;
} ImageRecvContext;

/* common helpers */
uint16_t img_crc16_ccitt(const uint8_t *data, size_t len);
void bitmap_set(uint8_t *bitmap, uint16_t index);
void bitmap_clear_bit(uint8_t *bitmap, uint16_t index);
int bitmap_is_set(const uint8_t *bitmap, uint16_t index);
void bitmap_clear_all(uint8_t *bitmap, size_t len);
uint16_t image_calc_total_chunks(uint32_t image_size);

/* sender API */
int sender_init(ImageSendContext *ctx, const char *image_path, uint32_t image_seq);
void sender_close(ImageSendContext *ctx);
int sender_build_chunk(ImageSendContext *ctx,
                       uint16_t chunk_index,
                       uint8_t flags,
                       ImageChunkPacket *pkt);
int sender_parse_ack(ImageSendContext *ctx, const ImageFullAckPacket *ack);
void sender_build_resend_list(ImageSendContext *ctx);
int sender_get_next_packet_for_cycle(ImageSendContext *ctx, ImageChunkPacket *pkt);
int sender_is_done(const ImageSendContext *ctx);

/* receiver API */
int receiver_init(ImageRecvContext *ctx,
                  const char *save_path,
                  uint32_t image_seq,
                  uint32_t image_size,
                  uint16_t total_chunks);
void receiver_close(ImageRecvContext *ctx);
int receiver_handle_chunk(ImageRecvContext *ctx, const ImageChunkPacket *pkt);
int receiver_build_ack(const ImageRecvContext *ctx,
                       uint16_t cycle_index,
                       uint8_t ack_seq,
                       ImageFullAckPacket *ack);
int receiver_is_done(const ImageRecvContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
