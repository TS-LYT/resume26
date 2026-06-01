#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "image_transfer.h"

uint16_t img_crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;

    for (size_t i = 0; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;

        for (int bit = 0; bit < 8; bit++)
        {
            if (crc & 0x8000u)
            {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

void bitmap_set(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8u] |= (uint8_t)(1u << (index % 8u));
}

void bitmap_clear_bit(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8u] &= (uint8_t)(~(1u << (index % 8u)));
}

int bitmap_is_set(const uint8_t *bitmap, uint16_t index)
{
    return (bitmap[index / 8u] & (uint8_t)(1u << (index % 8u))) != 0;
}

void bitmap_clear_all(uint8_t *bitmap, size_t len)
{
    memset(bitmap, 0, len);
}

uint16_t image_calc_total_chunks(uint32_t image_size)
{
    return (uint16_t)((image_size + IMAGE_CHUNK_DATA_SIZE - 1u) / IMAGE_CHUNK_DATA_SIZE);
}
