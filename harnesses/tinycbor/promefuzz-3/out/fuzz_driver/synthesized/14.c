// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_double at cbor.h:267:27 in cbor.h
// cbor_encode_float_as_half_float at cborencoder_float.c:32:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_double at cbor.h:267:27 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_double at cbor.h:267:27 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_double at cbor.h:267:27 in cbor.h
// cbor_encode_float_as_half_float at cborencoder_float.c:32:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *p, size_t size, size_t off) {
    uint16_t v = 0;
    if (off < size) v |= (uint16_t)p[off];
    if (off + 1 < size) v |= (uint16_t)p[off + 1] << 8;
    return v;
}

static uint32_t read_u32(const uint8_t *p, size_t size, size_t off) {
    uint32_t v = 0;
    if (off < size) v |= (uint32_t)p[off];
    if (off + 1 < size) v |= (uint32_t)p[off + 1] << 8;
    if (off + 2 < size) v |= (uint32_t)p[off + 2] << 16;
    if (off + 3 < size) v |= (uint32_t)p[off + 3] << 24;
    return v;
}

static uint64_t read_u64(const uint8_t *p, size_t size, size_t off) {
    uint64_t v = 0;
    if (off < size) v |= (uint64_t)p[off];
    if (off + 1 < size) v |= (uint64_t)p[off + 1] << 8;
    if (off + 2 < size) v |= (uint64_t)p[off + 2] << 16;
    if (off + 3 < size) v |= (uint64_t)p[off + 3] << 24;
    if (off + 4 < size) v |= (uint64_t)p[off + 4] << 32;
    if (off + 5 < size) v |= (uint64_t)p[off + 5] << 40;
    if (off + 6 < size) v |= (uint64_t)p[off + 6] << 48;
    if (off + 7 < size) v |= (uint64_t)p[off + 7] << 56;
    return v;
}

static float bits_to_float(uint32_t u) {
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double bits_to_double(uint64_t u) {
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    uint8_t stackbuf[128];
    uint8_t *buf = stackbuf;
    size_t bufsize = sizeof(stackbuf);

    if (Size > 0) {
        bufsize = (size_t)(Data[0] % 192);
        if (bufsize == 0)
            bufsize = 1;
    }

    if (bufsize > sizeof(stackbuf)) {
        buf = (uint8_t *)malloc(bufsize);
        if (!buf)
            return 0;
    }

    memset(buf, 0, bufsize);

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buf, bufsize, 0);

    uint16_t half_bits = read_u16(Data, Size, 1);
    uint32_t float_bits = read_u32(Data, Size, 3);
    uint64_t double_bits = read_u64(Data, Size, 7);
    float f = bits_to_float(float_bits);
    double d = bits_to_double(double_bits);

    (void)cbor_encode_half_float(&encoder, &half_bits);
    (void)cbor_encode_float(&encoder, f);
    (void)cbor_encode_double(&encoder, d);
    (void)cbor_encode_float_as_half_float(&encoder, f);

    {
        CborType types[3] = { CborHalfFloatType, CborFloatType, CborDoubleType };
        CborType t = types[(Size > 15 ? Data[15] : 0) % 3];
        if (t == CborHalfFloatType) {
            (void)cbor_encode_floating_point(&encoder, t, &half_bits);
        } else if (t == CborFloatType) {
            (void)cbor_encode_floating_point(&encoder, t, &f);
        } else {
            (void)cbor_encode_floating_point(&encoder, t, &d);
        }
    }

    {
        CborEncoder array;
        size_t count = (Size > 16) ? (size_t)(Data[16] % 4) : 0;
        CborError err = cbor_encoder_create_array(&encoder, &array, count);
        if (err == CborNoError) {
            size_t items = (Size > 17) ? (size_t)(Data[17] % 6) : 0;
            for (size_t i = 0; i < items; i++) {
                switch ((Size > 18 + i ? Data[18 + i] : 0) % 4) {
                    case 0:
                        (void)cbor_encode_half_float(&array, &half_bits);
                        break;
                    case 1:
                        (void)cbor_encode_float(&array, f);
                        break;
                    case 2:
                        (void)cbor_encode_double(&array, d);
                        break;
                    default:
                        (void)cbor_encode_float_as_half_float(&array, f);
                        break;
                }
            }
            (void)cbor_encoder_close_container(&encoder, &array);
        }
    }

    {
        CborEncoder map;
        size_t pairs = (Size > 24) ? (size_t)(Data[24] % 3) : 0;
        CborError err = cbor_encoder_create_map(&encoder, &map, pairs);
        if (err == CborNoError) {
            size_t actual_pairs = (Size > 25) ? (size_t)(Data[25] % 5) : 0;
            for (size_t i = 0; i < actual_pairs; i++) {
                (void)cbor_encode_int(&map, (int)i);
                switch ((Size > 26 + i ? Data[26 + i] : 0) % 3) {
                    case 0:
                        (void)cbor_encode_half_float(&map, &half_bits);
                        break;
                    case 1:
                        (void)cbor_encode_float(&map, f);
                        break;
                    default:
                        (void)cbor_encode_double(&map, d);
                        break;
                }
            }
            (void)cbor_encoder_close_container(&encoder, &map);
        }
    }

    {
        CborEncoder indef_array;
        CborError err = cbor_encoder_create_array(&encoder, &indef_array, CborIndefiniteLength);
        if (err == CborNoError) {
            (void)cbor_encode_half_float(&indef_array, &half_bits);
            (void)cbor_encode_float(&indef_array, f);
            (void)cbor_encode_double(&indef_array, d);
            (void)cbor_encoder_close_container(&encoder, &indef_array);
        }
    }

    if (buf != stackbuf)
        free(buf);
    return 0;
}