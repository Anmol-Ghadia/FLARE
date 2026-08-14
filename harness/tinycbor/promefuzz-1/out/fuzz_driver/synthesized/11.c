// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_raw at cborencoder.c:489:11 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static size_t get_size_t(const uint8_t *data, size_t size, size_t *offset) {
    size_t v = 0;
    size_t i;
    for (i = 0; i < sizeof(size_t); ++i) {
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

static size_t remaining_bytes(size_t size, size_t off) {
    return off <= size ? (size - off) : 0;
}

static size_t bounded_len(size_t requested, size_t available) {
    if (available == 0)
        return 0;
    return requested % (available + 1);
}

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    size_t out_cap, raw_len, bstr_len, array_len;
    size_t available;
    uint64_t u1, u2, u3;
    uint8_t *outbuf = NULL;
    FILE *fp = NULL;
    CborEncoder root, array;
    CborError err;

    if (Data == NULL)
        return 0;

    out_cap = get_size_t(Data, Size, &off) % 4096;
    outbuf = (uint8_t *)malloc(out_cap ? out_cap : 1);
    if (!outbuf)
        return 0;

    if (out_cap != 0)
        memset(outbuf, 0, out_cap);

    /* Use flags=0 to avoid writer-mode initialization, which can make
       append operations treat data.ptr as a callback and jump to it. */
    cbor_encoder_init(&root, outbuf, out_cap, 0);

    available = remaining_bytes(Size, off);
    raw_len = bounded_len(get_size_t(Data, Size, &off), available);
    available = remaining_bytes(Size, off);
    if (raw_len > available)
        raw_len = available;
    err = cbor_encode_raw(&root, Data + off, raw_len);
    (void)err;
    off += raw_len;

    available = remaining_bytes(Size, off);
    bstr_len = bounded_len(get_size_t(Data, Size, &off), available);
    available = remaining_bytes(Size, off);
    if (bstr_len > available)
        bstr_len = available;
    err = cbor_encode_byte_string(&root, Data + off, bstr_len);
    (void)err;
    off += bstr_len;

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

        available = remaining_bytes(Size, off);
        bstr_len = bounded_len(get_size_t(Data, Size, &off), available);
        available = remaining_bytes(Size, off);
        if (bstr_len > available)
            bstr_len = available;
        err = cbor_encode_byte_string(&array, Data + off, bstr_len);
        (void)err;
        off += bstr_len;

        available = remaining_bytes(Size, off);
        raw_len = bounded_len(get_size_t(Data, Size, &off), available);
        available = remaining_bytes(Size, off);
        if (raw_len > available)
            raw_len = available;
        err = cbor_encode_raw(&array, Data + off, raw_len);
        (void)err;
        off += raw_len;

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
        if (Size != 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    free(outbuf);
    return 0;
}