#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

static CborError dummy_writer(void *token, const void *data, size_t len, CborEncoderAppendType type)
{
    (void)type;
    uint8_t *buf = (uint8_t *)token;
    if (buf && data && len) {
        size_t copy = len > 4096 ? 4096 : len;
        memcpy(buf, data, copy);
    }
    return CborNoError;
}

static size_t take_size(const uint8_t **p, size_t *n)
{
    size_t v = 0;
    size_t count = *n < sizeof(size_t) ? *n : sizeof(size_t);
    for (size_t i = 0; i < count; ++i) {
        v = (v << 8) ^ (*p)[i];
    }
    *p += count;
    *n -= count;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *p = Data;
    size_t n = Size;

    int mode = 0;
    int flags = 0;
    size_t bufcap = 0;
    size_t rawlen1 = 0, rawlen2 = 0;
    size_t arrlen = 0, maplen = 0;
    uint64_t tag1 = 0, tag2 = 0;

    if (n > 0) {
        mode = *p++;
        --n;
    }
    if (n > 0) {
        flags = (int)(*p++ & 0x0f);
        --n;
    }

    bufcap = take_size(&p, &n);
    rawlen1 = take_size(&p, &n);
    rawlen2 = take_size(&p, &n);
    arrlen = take_size(&p, &n);
    maplen = take_size(&p, &n);
    tag1 = (uint64_t)take_size(&p, &n);
    tag2 = (uint64_t)take_size(&p, &n);

    bufcap %= 8192;
    rawlen1 %= (n + 1);
    rawlen2 %= (n + 1);

    if (bufcap == 0)
        bufcap = 1;

    uint8_t *buffer = (uint8_t *)malloc(bufcap);
    if (!buffer)
        return 0;
    memset(buffer, 0, bufcap);

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborEncoder enc;
    CborEncoder child1;
    CborEncoder child2;
    memset(&enc, 0, sizeof(enc));
    memset(&child1, 0, sizeof(child1));
    memset(&child2, 0, sizeof(child2));

    if (mode & 1)
        cbor_encoder_init(&enc, buffer, bufcap, flags);
    else
        cbor_encoder_init_writer(&enc, dummy_writer, buffer);

    if (mode & 2)
        (void)cbor_encode_tag(&enc, (CborTag)tag1);

    if (mode & 4) {
        size_t len = (arrlen & 1) ? CborIndefiniteLength : (arrlen % 16);
        if (cbor_encoder_create_array(&enc, &child1, len) == CborNoError) {
            if (rawlen1 && n >= rawlen1)
                (void)cbor_encode_raw(&child1, p, rawlen1);
            else
                (void)cbor_encode_raw(&child1, p, n);

            (void)cbor_encode_tag(&child1, (CborTag)tag2);

            if (mode & 8) {
                size_t mlen = (maplen & 1) ? CborIndefiniteLength : (maplen % 16);
                if (cbor_encoder_create_map(&child1, &child2, mlen) == CborNoError) {
                    if (rawlen2 && n >= rawlen2)
                        (void)cbor_encode_raw(&child2, p, rawlen2);
                    else
                        (void)cbor_encode_raw(&child2, p, n);

                    (void)cbor_encode_tag(&child2, (CborTag)(tag1 ^ tag2));
                }
            }
        }
    } else {
        size_t mlen = (maplen & 1) ? CborIndefiniteLength : (maplen % 16);
        if (cbor_encoder_create_map(&enc, &child1, mlen) == CborNoError) {
            if (rawlen1 && n >= rawlen1)
                (void)cbor_encode_raw(&child1, p, rawlen1);
            else
                (void)cbor_encode_raw(&child1, p, n);

            (void)cbor_encode_tag(&child1, (CborTag)tag2);

            if (mode & 16) {
                size_t alen = (arrlen & 1) ? CborIndefiniteLength : (arrlen % 16);
                if (cbor_encoder_create_array(&child1, &child2, alen) == CborNoError) {
                    if (rawlen2 && n >= rawlen2)
                        (void)cbor_encode_raw(&child2, p, rawlen2);
                    else
                        (void)cbor_encode_raw(&child2, p, n);

                    (void)cbor_encode_tag(&child2, (CborTag)(tag1 + tag2));
                }
            }
        }
    }

    if (n > 0) {
        size_t tail = n % (n + 1);
        (void)cbor_encode_raw(&enc, p, tail);
    } else {
        (void)cbor_encode_raw(&enc, (const uint8_t *)"", 0);
    }

    free(buffer);
    return 0;
}