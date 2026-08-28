#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

static CborError dummy_writer(void *token, const void *data, size_t len, CborEncoderAppendType type)
{
    (void)token;
    (void)data;
    (void)len;
    (void)type;
    return CborNoError;
}

static size_t take_size(const uint8_t **p, size_t *n)
{
    size_t v = 0;
    size_t count = *n < sizeof(size_t) ? *n : sizeof(size_t);
    for (size_t i = 0; i < count; ++i)
        v = (v << 8) ^ (*p)[i];
    *p += count;
    *n -= count;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    const uint8_t *p = Data;
    size_t n = Size;

    int mode = 0;
    size_t bufcap, rawlen1, rawlen2, arrlen, maplen;
    uint64_t tag1, tag2;

    if (n > 0) {
        mode = *p++;
        --n;
    }

    bufcap = take_size(&p, &n) % 8192;
    rawlen1 = take_size(&p, &n);
    rawlen2 = take_size(&p, &n);
    arrlen = take_size(&p, &n);
    maplen = take_size(&p, &n);
    tag1 = (uint64_t)take_size(&p, &n);
    tag2 = (uint64_t)take_size(&p, &n);

    if (bufcap == 0)
        bufcap = 1;

    if (rawlen1 > n)
        rawlen1 = n;
    if (rawlen2 > n)
        rawlen2 = n;

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
        cbor_encoder_init_writer(&enc, dummy_writer, NULL);
    else
        cbor_encoder_init(&enc, buffer, bufcap, 0);

    (void)cbor_encode_tag(&enc, (CborTag)tag1);
    (void)cbor_encode_raw(&enc, p, rawlen1);

    {
        size_t alen = (arrlen & 1) ? CborIndefiniteLength : (arrlen % 16);
        if (cbor_encoder_create_array(&enc, &child1, alen) == CborNoError) {
            (void)cbor_encode_tag(&child1, (CborTag)tag2);
            (void)cbor_encode_raw(&child1, p, rawlen2);
        }
    }

    {
        size_t mlen = (maplen & 1) ? CborIndefiniteLength : (maplen % 16);
        if (cbor_encoder_create_map(&enc, &child1, mlen) == CborNoError) {
            (void)cbor_encode_tag(&child1, (CborTag)(tag1 ^ tag2));
            (void)cbor_encode_raw(&child1, p, rawlen1);

            if (mode & 2) {
                size_t nested_alen = ((arrlen >> 1) & 1) ? CborIndefiniteLength : ((arrlen >> 1) % 8);
                if (cbor_encoder_create_array(&child1, &child2, nested_alen) == CborNoError) {
                    (void)cbor_encode_tag(&child2, (CborTag)(tag1 + tag2));
                    (void)cbor_encode_raw(&child2, p, rawlen2);
                }
            }
        }
    }

    if (n > 0)
        (void)cbor_encode_raw(&enc, p, n);

    free(buffer);
    return 0;
}