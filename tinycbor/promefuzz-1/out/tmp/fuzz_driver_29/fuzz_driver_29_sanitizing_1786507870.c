#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *off) {
    uint16_t v = 0;
    if (*off < size) v = (uint16_t)(v | ((uint16_t)data[(*off)++] << 8));
    if (*off < size) v = (uint16_t)(v | (uint16_t)data[(*off)++]);
    return v;
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    if (*off < size) v |= (uint32_t)data[(*off)++] << 24;
    if (*off < size) v |= (uint32_t)data[(*off)++] << 16;
    if (*off < size) v |= (uint32_t)data[(*off)++] << 8;
    if (*off < size) v |= (uint32_t)data[(*off)++];
    return v;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *off) {
    uint64_t hi = read_u32(data, size, off);
    uint64_t lo = read_u32(data, size, off);
    return (hi << 32) | lo;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    int flags = 0;
    if (off < Size)
        flags = (int)(Data[off++] & 0x0f);

    size_t out_size = 0;
    if (off < Size)
        out_size = (size_t)Data[off++];
    if (off < Size)
        out_size = (out_size << 8) ^ (size_t)Data[off++];

    if (out_size > 4096)
        out_size = 4096;

    if (out_size == 0)
        out_size = 1;

    uint8_t *buffer = (uint8_t *)malloc(out_size);
    if (!buffer)
        return 0;

    memset(buffer, 0, out_size);

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        CborEncoder encoder;
        cbor_encoder_init(&encoder, buffer, out_size, flags);

        (void)cbor_encoder_get_extra_bytes_needed(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);

        for (int round = 0; round < 3; ++round) {
            CborEncoder mapEncoder;
            CborError err;
            size_t length_choice;

            if (round == 0) {
                length_choice = 0;
            } else if (round == 1) {
                length_choice = CborIndefiniteLength;
            } else {
                uint16_t raw = read_u16(Data, Size, &off);
                length_choice = (size_t)(raw % 32);
                if ((raw & 0x8000u) != 0)
                    length_choice = CborIndefiniteLength;
            }

            err = cbor_encoder_create_map(&encoder, &mapEncoder, length_choice);
            (void)err;
            (void)cbor_encoder_get_extra_bytes_needed(&encoder);
            (void)cbor_encoder_get_buffer_size(&encoder, buffer);

            if (err == CborNoError) {
                int items = 0;
                if (off < Size)
                    items = (int)(Data[off++] % 8);

                for (int i = 0; i < items; ++i) {
                    uint64_t absval = ((i & 1) == 0) ? read_u64(Data, Size, &off)
                                                     : (uint64_t)read_u32(Data, Size, &off);
                    (void)cbor_encode_negative_int(&mapEncoder, absval);
                    (void)cbor_encoder_get_extra_bytes_needed(&mapEncoder);
                    (void)cbor_encoder_get_buffer_size(&mapEncoder, buffer);
                }

                (void)cbor_encoder_close_container(&encoder, &mapEncoder);
                (void)cbor_encoder_get_extra_bytes_needed(&encoder);
                (void)cbor_encoder_get_buffer_size(&encoder, buffer);
            }
        }
    }

    {
        CborEncoder encoder2;
        cbor_encoder_init(&encoder2, buffer, out_size, flags);

        int count = 1;
        if (off < Size)
            count = 1 + (int)(Data[off++] % 16);

        for (int i = 0; i < count; ++i) {
            uint64_t absval = read_u64(Data, Size, &off);
            (void)cbor_encode_negative_int(&encoder2, absval);
            (void)cbor_encoder_get_extra_bytes_needed(&encoder2);
            (void)cbor_encoder_get_buffer_size(&encoder2, buffer);
        }
    }

    free(buffer);
    return 0;
}