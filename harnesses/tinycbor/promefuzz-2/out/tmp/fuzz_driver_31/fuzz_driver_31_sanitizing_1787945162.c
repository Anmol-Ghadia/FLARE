#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

static float u32_to_float(uint32_t u) {
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double u64_to_double(uint64_t u) {
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

static void init_encoder(CborEncoder *enc, uint8_t *buf, size_t len) {
    cbor_encoder_init(enc, buf, len, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    size_t buf_size = Size ? Size : 1;
    uint8_t *buf = (uint8_t *)malloc(buf_size);
    if (!buf)
        return 0;

    memset(buf, 0, buf_size);

    CborEncoder encoder;
    CborError err;

    init_encoder(&encoder, buf, buf_size);
    err = cbor_encode_undefined(&encoder);
    (void)err;

    init_encoder(&encoder, buf, buf_size);
    {
        uint8_t simple = Size > 0 ? Data[0] : 0;
        err = cbor_encode_simple_value(&encoder, simple);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        float f = u32_to_float(read_u32(Data, Size, 1));
        err = cbor_encode_float(&encoder, f);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        float f = u32_to_float(read_u32(Data, Size, 5));
        err = cbor_encode_float_as_half_float(&encoder, f);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        double d = u64_to_double(read_u64(Data, Size, 9));
        err = cbor_encode_double(&encoder, d);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        uint16_t raw16 = read_u16(Data, Size, 17);
        err = cbor_encode_floating_point(&encoder, CborHalfFloatType, &raw16);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        uint32_t raw32 = read_u32(Data, Size, 19);
        err = cbor_encode_floating_point(&encoder, CborFloatType, &raw32);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        uint64_t raw64 = read_u64(Data, Size, 23);
        err = cbor_encode_floating_point(&encoder, CborDoubleType, &raw64);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        uint64_t raw64 = read_u64(Data, Size, 31);
        CborType t;
        switch (Size > 0 ? (Data[0] % 6) : 0) {
            case 0: t = CborHalfFloatType; break;
            case 1: t = CborFloatType; break;
            case 2: t = CborDoubleType; break;
            case 3: t = CborSimpleType; break;
            case 4: t = CborInvalidType; break;
            default: t = (CborType)(Size > 1 ? Data[1] : 0xff); break;
        }
        err = cbor_encode_floating_point(&encoder, t, &raw64);
        (void)err;
    }

    init_encoder(&encoder, buf, buf_size);
    {
        size_t i = 0;
        while (i < Size) {
            switch (Data[i] % 6) {
                case 0:
                    err = cbor_encode_undefined(&encoder);
                    break;
                case 1:
                    err = cbor_encode_simple_value(&encoder, Data[i]);
                    break;
                case 2: {
                    float f = u32_to_float(read_u32(Data, Size, i));
                    err = cbor_encode_float(&encoder, f);
                    break;
                }
                case 3: {
                    float f = u32_to_float(read_u32(Data, Size, i));
                    err = cbor_encode_float_as_half_float(&encoder, f);
                    break;
                }
                case 4: {
                    double d = u64_to_double(read_u64(Data, Size, i));
                    err = cbor_encode_double(&encoder, d);
                    break;
                }
                default: {
                    uint8_t sel = Data[i];
                    if ((sel & 1) == 0) {
                        uint16_t v16 = read_u16(Data, Size, i);
                        err = cbor_encode_floating_point(&encoder, CborHalfFloatType, &v16);
                    } else if ((sel & 2) == 0) {
                        uint32_t v32 = read_u32(Data, Size, i);
                        err = cbor_encode_floating_point(&encoder, CborFloatType, &v32);
                    } else {
                        uint64_t v64 = read_u64(Data, Size, i);
                        err = cbor_encode_floating_point(&encoder, CborDoubleType, &v64);
                    }
                    break;
                }
            }
            (void)err;
            i++;
        }
    }

    free(buf);
    return 0;
}