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

static size_t get_size_t(const uint8_t *data, size_t size, size_t *offset) {
    size_t v = 0;
    size_t n = sizeof(size_t);
    size_t i;
    for (i = 0; i < n; ++i) {
        v <<= 8;
        if (*offset < size)
            v |= data[(*offset)++];
    }
    return v;
}

static uint64_t get_u64(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8; ++i) {
        v <<= 8;
        if (*offset < size)
            v |= data[(*offset)++];
    }
    return v;
}

static int get_int(const uint8_t *data, size_t size, size_t *offset) {
    int v = 0;
    size_t i;
    for (i = 0; i < sizeof(int); ++i) {
        v <<= 8;
        if (*offset < size)
            v |= data[(*offset)++];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    size_t out_cap, raw_len, bstr_len, array_len;
    uint64_t u1, u2, u3;
    int flags;
    uint8_t *outbuf = NULL;
    FILE *fp = NULL;
    CborEncoder root, array;
    CborError err;

    if (Size == 0)
        return 0;

    out_cap = get_size_t(Data, Size, &off);
    out_cap = out_cap % 4096;

    flags = get_int(Data, Size, &off);

    outbuf = (uint8_t *)malloc(out_cap ? out_cap : 1);
    if (!outbuf)
        return 0;

    if (out_cap && off < Size) {
        size_t seed = Size - off;
        if (seed > out_cap)
            seed = out_cap;
        memcpy(outbuf, Data + off, seed);
    }

    cbor_encoder_init(&root, outbuf, out_cap, flags);

    raw_len = get_size_t(Data, Size, &off);
    raw_len = (Size - off == 0) ? 0 : (raw_len % (Size - off + 1));
    err = cbor_encode_raw(&root, Data + off, raw_len);
    (void)err;
    if (off + raw_len <= Size)
        off += raw_len;
    else
        off = Size;

    bstr_len = get_size_t(Data, Size, &off);
    bstr_len = (Size - off == 0) ? 0 : (bstr_len % (Size - off + 1));
    err = cbor_encode_byte_string(&root, Data + off, bstr_len);
    (void)err;
    if (off + bstr_len <= Size)
        off += bstr_len;
    else
        off = Size;

    u1 = get_u64(Data, Size, &off);
    err = cbor_encode_uint(&root, u1);
    (void)err;

    array_len = get_size_t(Data, Size, &off);
    if (array_len & 1)
        array_len = CborIndefiniteLength;
    else
        array_len %= 8;

    err = cbor_encoder_create_array(&root, &array, array_len);
    if (err == CborNoError) {
        u2 = get_u64(Data, Size, &off);
        err = cbor_encode_uint(&array, u2);
        (void)err;

        bstr_len = get_size_t(Data, Size, &off);
        bstr_len = (Size - off == 0) ? 0 : (bstr_len % (Size - off + 1));
        err = cbor_encode_byte_string(&array, Data + off, bstr_len);
        (void)err;
        if (off + bstr_len <= Size)
            off += bstr_len;
        else
            off = Size;

        raw_len = get_size_t(Data, Size, &off);
        raw_len = (Size - off == 0) ? 0 : (raw_len % (Size - off + 1));
        err = cbor_encode_raw(&array, Data + off, raw_len);
        (void)err;
        if (off + raw_len <= Size)
            off += raw_len;
        else
            off = Size;

        u3 = get_u64(Data, Size, &off);
        err = cbor_encode_uint(&array, u3);
        (void)err;

        err = cbor_encoder_close_container(&root, &array);
        (void)err;
    }

    cbor_encoder_init(&root, outbuf, out_cap, 0);
    err = cbor_encoder_create_array(&root, &array, 1);
    if (err == CborNoError) {
        err = cbor_encode_uint(&array, get_u64(Data, Size, &off));
        (void)err;
        err = cbor_encode_uint(&array, get_u64(Data, Size, &off));
        (void)err;
        err = cbor_encoder_close_container(&root, &array);
        (void)err;
    }

    cbor_encoder_init(&root, outbuf, out_cap, 0);
    err = cbor_encoder_create_array(&root, &array, 3);
    if (err == CborNoError) {
        err = cbor_encode_uint(&array, get_u64(Data, Size, &off));
        (void)err;
        err = cbor_encoder_close_container(&root, &array);
        (void)err;
    }

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    free(outbuf);
    return 0;
}