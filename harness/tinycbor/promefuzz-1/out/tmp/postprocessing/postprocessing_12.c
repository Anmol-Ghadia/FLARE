#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *p, size_t size, size_t *off) {
    uint16_t v = 0;
    if (*off < size) v |= (uint16_t)p[(*off)++] << 8;
    if (*off < size) v |= (uint16_t)p[(*off)++];
    return v;
}

static uint32_t read_u32(const uint8_t *p, size_t size, size_t *off) {
    uint32_t v = 0;
    if (*off < size) v |= (uint32_t)p[(*off)++] << 24;
    if (*off < size) v |= (uint32_t)p[(*off)++] << 16;
    if (*off < size) v |= (uint32_t)p[(*off)++] << 8;
    if (*off < size) v |= (uint32_t)p[(*off)++];
    return v;
}

static float u32_to_float(uint32_t u) {
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;

    size_t bufsize = Size + 64;
    if (bufsize < 64)
        bufsize = 64;

    uint8_t *buffer = (uint8_t *)malloc(bufsize);
    if (!buffer)
        return 0;

    memset(buffer, 0, bufsize);

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, bufsize, 0);

    while (off < Size) {
        uint8_t op = Data[off++];

        switch (op % 10) {
        case 0: {
            (void)cbor_encode_null(&encoder);
            break;
        }
        case 1: {
            uint8_t value = 0;
            if (off < Size)
                value = Data[off++];
            (void)cbor_encode_simple_value(&encoder, value);
            break;
        }
        case 2: {
            uint32_t bits = read_u32(Data, Size, &off);
            float f = u32_to_float(bits);
            (void)cbor_encode_float(&encoder, f);
            break;
        }
        case 3: {
            uint16_t half = read_u16(Data, Size, &off);
            (void)cbor_encode_half_float(&encoder, &half);
            break;
        }
        case 4: {
            bool b = false;
            if (off < Size)
                b = (Data[off++] & 1u) != 0;
            (void)cbor_encode_boolean(&encoder, b);
            break;
        }
        case 5: {
            (void)cbor_encode_undefined(&encoder);
            break;
        }
        case 6: {
            (void)cbor_encode_boolean(&encoder, false);
            (void)cbor_encode_boolean(&encoder, true);
            (void)cbor_encode_null(&encoder);
            (void)cbor_encode_undefined(&encoder);
            break;
        }
        case 7: {
            uint8_t value = 0;
            if (off < Size)
                value = Data[off++];
            (void)cbor_encode_simple_value(&encoder, (uint8_t)(value & 0x1f));
            (void)cbor_encode_simple_value(&encoder, value);
            break;
        }
        case 8: {
            uint32_t bits1 = read_u32(Data, Size, &off);
            uint32_t bits2 = read_u32(Data, Size, &off);
            float f1 = u32_to_float(bits1);
            float f2 = u32_to_float(bits2);
            (void)cbor_encode_float(&encoder, f1);
            (void)cbor_encode_float(&encoder, f2);
            break;
        }
        case 9: {
            uint16_t half1 = read_u16(Data, Size, &off);
            uint16_t half2 = read_u16(Data, Size, &off);
            (void)cbor_encode_half_float(&encoder, &half1);
            (void)cbor_encode_half_float(&encoder, &half2);
            break;
        }
        default:
            break;
        }
    }

    free(buffer);
    return 0;
}