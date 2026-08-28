// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *offset < size; ++i, ++(*offset))
        v = (v << 8) | data[*offset];
    return v;
}

static float read_float(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t bits = 0;
    float f = 0.0f;
    size_t i;
    for (i = 0; i < 4 && *offset < size; ++i, ++(*offset))
        bits = (bits << 8) | data[*offset];
    memcpy(&f, &bits, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput_34(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    size_t bufsize;
    int flags;
    size_t map_len_selector;
    size_t item_budget;
    uint8_t *buffer;
    CborEncoder encoder;
    CborEncoder mapEncoder;
    CborError err;
    size_t i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (Size == 0) {
        uint8_t tinybuf[1];
        cbor_encoder_init(&encoder, tinybuf, sizeof(tinybuf), 0);
        (void)cbor_encoder_get_extra_bytes_needed(&encoder);
        return 0;
    }

    bufsize = (off < Size) ? (size_t)Data[off++] : 0;
    if (off < Size)
        bufsize |= ((size_t)Data[off++] << 8);
    bufsize %= 512;
    if (bufsize == 0)
        bufsize = 1;

    flags = 0;
    map_len_selector = (off < Size) ? (size_t)Data[off++] : 0;
    item_budget = (off < Size) ? (size_t)Data[off++] : 0;
    item_budget %= 32;

    buffer = (uint8_t *)malloc(bufsize);
    if (!buffer)
        return 0;
    memset(buffer, 0, bufsize);

    cbor_encoder_init(&encoder, buffer, bufsize, flags);
    (void)cbor_encoder_get_extra_bytes_needed(&encoder);

    {
        size_t map_length;
        if ((map_len_selector & 1) != 0)
            map_length = CborIndefiniteLength;
        else
            map_length = map_len_selector % 8;

        err = cbor_encoder_create_map(&encoder, &mapEncoder, map_length);
        (void)cbor_encoder_get_extra_bytes_needed(&encoder);

        if (err == CborNoError) {
            size_t max_items;
            if (map_length == CborIndefiniteLength) {
                max_items = item_budget;
            } else {
                max_items = map_length * 2;
                if (item_budget < max_items)
                    max_items = item_budget;
            }

            for (i = 0; i < max_items && off < Size; ++i) {
                uint8_t op = Data[off++];

                if (op & 1) {
                    uint64_t v = read_u64(Data, Size, &off);
                    (void)cbor_encode_uint(&mapEncoder, v);
                } else {
                    float f = read_float(Data, Size, &off);
                    (void)cbor_encode_float(&mapEncoder, f);
                }

                (void)cbor_encoder_get_extra_bytes_needed(&mapEncoder);
                (void)cbor_encoder_get_extra_bytes_needed(&encoder);
            }

            (void)cbor_encoder_close_container(&encoder, &mapEncoder);
            (void)cbor_encoder_get_extra_bytes_needed(&encoder);
        }
    }

    if (off < Size) {
        size_t extra_ops = Data[off++] % 2;
        for (i = 0; i < extra_ops && off < Size; ++i) {
            uint8_t op = Data[off++];
            if (op & 1) {
                uint64_t v = read_u64(Data, Size, &off);
                (void)cbor_encode_uint(&encoder, v);
            } else {
                float f = read_float(Data, Size, &off);
                (void)cbor_encode_float(&encoder, f);
            }
            (void)cbor_encoder_get_extra_bytes_needed(&encoder);
        }
    }

    free(buffer);
    return 0;
}