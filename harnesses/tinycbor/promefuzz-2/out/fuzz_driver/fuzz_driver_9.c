// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_float_as_half_float at cborencoder_float.c:32:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_half_float at cbor.h:262:27 in cbor.h
// cbor_encode_float at cbor.h:265:27 in cbor.h
// cbor_encode_float_as_half_float at cborencoder_float.c:32:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
// cbor_encode_floating_point at cborencoder.c:409:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint8_t out_small[1];
    uint8_t out_med[16];
    uint8_t out_large[256];
    uint8_t out_zero[1];

    CborEncoder enc_small, enc_med, enc_large, enc_zero;
    cbor_encoder_init(&enc_small, out_small, sizeof(out_small), 0);
    cbor_encoder_init(&enc_med, out_med, sizeof(out_med), 0);
    cbor_encoder_init(&enc_large, out_large, sizeof(out_large), 0);
    cbor_encoder_init(&enc_zero, out_zero, 0, 0);

    bool b = (Size > 0) ? ((Data[0] & 1u) != 0) : false;
    uint16_t half_bits = read_u16(Data, Size, 1);
    uint32_t float_bits = read_u32(Data, Size, 3);
    uint64_t double_bits = read_u64(Data, Size, 7);
    float f = u32_to_float(float_bits);
    CborTag tag = (CborTag)read_u64(Data, Size, 15);

    CborEncoder *encoders[4] = { &enc_small, &enc_med, &enc_large, &enc_zero };
    size_t i;

    for (i = 0; i < 4; ++i) {
        CborError err;

        err = cbor_encode_boolean(encoders[i], b);
        (void)err;

        err = cbor_encode_half_float(encoders[i], &half_bits);
        (void)err;

        err = cbor_encode_float(encoders[i], f);
        (void)err;

        err = cbor_encode_float_as_half_float(encoders[i], f);
        (void)err;

        err = cbor_encode_tag(encoders[i], tag);
        (void)err;

        err = cbor_encode_floating_point(encoders[i], CborHalfFloatType, &half_bits);
        (void)err;

        err = cbor_encode_floating_point(encoders[i], CborFloatType, &float_bits);
        (void)err;

        err = cbor_encode_floating_point(encoders[i], CborDoubleType, &double_bits);
        (void)err;
    }

    if (Size >= 2) {
        CborEncoder dyn;
        size_t cap = (size_t)(Data[0] % 64);
        uint8_t buf[64];
        cbor_encoder_init(&dyn, buf, cap, 0);

        (void)cbor_encode_tag(&dyn, (CborTag)Data[1]);
        (void)cbor_encode_boolean(&dyn, (Data[0] & 2u) != 0);
        (void)cbor_encode_half_float(&dyn, &half_bits);
        (void)cbor_encode_float(&dyn, f);
        (void)cbor_encode_float_as_half_float(&dyn, -f);
        (void)cbor_encode_floating_point(&dyn, CborHalfFloatType, &half_bits);
        (void)cbor_encode_floating_point(&dyn, CborFloatType, &float_bits);
        (void)cbor_encode_floating_point(&dyn, CborDoubleType, &double_bits);
    }

    return 0;
}