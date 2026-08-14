#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t read_u64(const uint8_t *p) {
    return ((uint64_t)p[0]) |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t stackbuf[128];
    uint8_t *heapbuf = NULL;
    uint8_t *buf = stackbuf;
    size_t bufsize = sizeof(stackbuf);
    size_t offset = 0;
    CborEncoder encoder;
    CborError err;
    uint8_t *ptr;

    if (Size > 0) {
        bufsize = (size_t)(Data[0] % 192);
        offset = 1;
        if (bufsize == 0)
            bufsize = 1;
    }

    if (bufsize > sizeof(stackbuf)) {
        heapbuf = (uint8_t *)malloc(bufsize);
        if (!heapbuf)
            return 0;
        buf = heapbuf;
    }
    memset(buf, 0, bufsize);

    cbor_encoder_init(&encoder, buf, bufsize, 0);

    ptr = _cbor_encoder_get_buffer_pointer(&encoder);
    (void)ptr;

    while (offset < Size) {
        uint8_t op = Data[offset++];

        switch (op % 8) {
        case 0: {
            float f = 0.0f;
            if (Size - offset >= 4) {
                uint32_t bits = read_u32(Data + offset);
                memcpy(&f, &bits, sizeof(f));
                offset += 4;
            } else if (Size > offset) {
                uint32_t bits = 0;
                size_t rem = Size - offset;
                for (size_t i = 0; i < rem; ++i)
                    bits |= ((uint32_t)Data[offset + i]) << (8 * i);
                memcpy(&f, &bits, sizeof(f));
                offset = Size;
            }
            err = cbor_encode_float(&encoder, f);
            (void)err;
            break;
        }

        case 1: {
            double d = 0.0;
            if (Size - offset >= 8) {
                uint64_t bits = read_u64(Data + offset);
                memcpy(&d, &bits, sizeof(d));
                offset += 8;
            } else if (Size > offset) {
                uint64_t bits = 0;
                size_t rem = Size - offset;
                for (size_t i = 0; i < rem; ++i)
                    bits |= ((uint64_t)Data[offset + i]) << (8 * i);
                memcpy(&d, &bits, sizeof(d));
                offset = Size;
            }
            err = cbor_encode_double(&encoder, d);
            (void)err;
            break;
        }

        case 2: {
            float f = 0.0f;
            if (Size - offset >= 4) {
                uint32_t bits = read_u32(Data + offset);
                memcpy(&f, &bits, sizeof(f));
                offset += 4;
            } else if (Size > offset) {
                uint32_t bits = 0;
                size_t rem = Size - offset;
                for (size_t i = 0; i < rem; ++i)
                    bits |= ((uint32_t)Data[offset + i]) << (8 * i);
                memcpy(&f, &bits, sizeof(f));
                offset = Size;
            }
            err = cbor_encode_float_as_half_float(&encoder, f);
            (void)err;
            break;
        }

        case 3: {
            uint16_t half = 0;
            if (Size - offset >= 2) {
                half = read_u16(Data + offset);
                offset += 2;
            } else if (Size > offset) {
                half = Data[offset];
                offset = Size;
            }
            err = cbor_encode_half_float(&encoder, &half);
            (void)err;
            break;
        }

        case 4: {
            uint16_t half = 0;
            if (Size - offset >= 2) {
                half = read_u16(Data + offset);
                offset += 2;
            } else if (Size > offset) {
                half = Data[offset];
                offset = Size;
            }
            err = cbor_encode_floating_point(&encoder, CborHalfFloatType, &half);
            (void)err;
            break;
        }

        case 5: {
            uint32_t bits = 0;
            if (Size - offset >= 4) {
                bits = read_u32(Data + offset);
                offset += 4;
            } else if (Size > offset) {
                size_t rem = Size - offset;
                for (size_t i = 0; i < rem; ++i)
                    bits |= ((uint32_t)Data[offset + i]) << (8 * i);
                offset = Size;
            }
            err = cbor_encode_floating_point(&encoder, CborFloatType, &bits);
            (void)err;
            break;
        }

        case 6: {
            uint64_t bits = 0;
            if (Size - offset >= 8) {
                bits = read_u64(Data + offset);
                offset += 8;
            } else if (Size > offset) {
                size_t rem = Size - offset;
                for (size_t i = 0; i < rem; ++i)
                    bits |= ((uint64_t)Data[offset + i]) << (8 * i);
                offset = Size;
            }
            err = cbor_encode_floating_point(&encoder, CborDoubleType, &bits);
            (void)err;
            break;
        }

        case 7: {
            ptr = _cbor_encoder_get_buffer_pointer(&encoder);
            if (ptr && bufsize > 0) {
                volatile uint8_t sink = ptr >= buf && ptr < buf + bufsize ? *ptr : 0;
                (void)sink;
            }

            if (Size - offset >= 1) {
                FILE *fp = fopen("./dummy_file", "wb");
                if (fp) {
                    size_t to_write = Data[offset] % 32;
                    offset++;
                    if (to_write > Size - offset)
                        to_write = Size - offset;
                    if (to_write)
                        fwrite(Data + offset, 1, to_write, fp);
                    fclose(fp);
                    offset += to_write;
                }
            }
            break;
        }
        }
    }

    ptr = _cbor_encoder_get_buffer_pointer(&encoder);
    (void)ptr;

    free(heapbuf);
    return 0;
}