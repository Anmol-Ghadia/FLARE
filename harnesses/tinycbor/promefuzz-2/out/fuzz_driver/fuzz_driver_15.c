// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// _cbor_encoder_get_buffer_pointer at cbor.h:275:26 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

static size_t get_size_t(const uint8_t *data, size_t size, size_t *offset) {
    size_t v = 0;
    size_t n = sizeof(size_t);
    size_t i;
    for (i = 0; i < n; i++) {
        v <<= 8;
        if (*offset < size)
            v |= data[(*offset)++];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t offset = 0;
    size_t out_size;
    uint8_t *buffer = NULL;
    CborEncoder encoder;
    CborEncoder child1, child2;
    CborError err1, err2, err3, err4;
    size_t len1, len2;
    size_t used1, used2, extra1, extra2;
    uint8_t *ptr1, *ptr2;
    int mode;

    if (Size == 0)
        return 0;

    out_size = get_size_t(Data, Size, &offset);
    if (out_size > 4096)
        out_size %= 4096;

    buffer = (uint8_t *)malloc(out_size ? out_size : 1);
    if (!buffer)
        return 0;

    if (out_size)
        memset(buffer, 0, out_size);

    cbor_encoder_init(&encoder, buffer, out_size, 0);

    mode = (offset < Size) ? (Data[offset++] % 6) : 0;

    len1 = get_size_t(Data, Size, &offset);
    len2 = get_size_t(Data, Size, &offset);

    if (mode == 0) {
        if (len1 % 4 == 0)
            len1 = CborIndefiniteLength;
        else
            len1 %= 16;

        err1 = cbor_encoder_create_array(&encoder, &child1, len1);
        used1 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr1 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err2 = cbor_encoder_close_container(&encoder, &child1);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    } else if (mode == 1) {
        if (len1 % 4 == 0)
            len1 = CborIndefiniteLength;
        else
            len1 %= 16;

        err1 = cbor_encoder_create_map(&encoder, &child1, len1);
        used1 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr1 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err2 = cbor_encoder_close_container(&encoder, &child1);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    } else if (mode == 2) {
        len1 = (len1 % 8);
        len2 = (len2 % 8);

        err1 = cbor_encoder_create_array(&encoder, &child1, len1);
        err2 = cbor_encoder_create_map(&child1, &child2, len2);
        used1 = cbor_encoder_get_buffer_size(&child1, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&child1);
        ptr1 = _cbor_encoder_get_buffer_pointer(&child1);
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err3 = cbor_encoder_close_container(&child1, &child2);
        err4 = cbor_encoder_close_container(&encoder, &child1);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)err3;
        (void)err4;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    } else if (mode == 3) {
        if (len1 % 2)
            len1 = CborIndefiniteLength;
        else
            len1 %= 8;

        if (len2 % 2)
            len2 = CborIndefiniteLength;
        else
            len2 %= 8;

        err1 = cbor_encoder_create_map(&encoder, &child1, len1);
        err2 = cbor_encoder_create_array(&child1, &child2, len2);
        used1 = cbor_encoder_get_buffer_size(&child2, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&child2);
        ptr1 = _cbor_encoder_get_buffer_pointer(&child2);
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err3 = cbor_encoder_close_container(&child1, &child2);
        err4 = cbor_encoder_close_container(&encoder, &child1);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)err3;
        (void)err4;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    } else if (mode == 4) {
        len1 = (size_t)-1;
        len2 = (size_t)-1;

        err1 = cbor_encoder_create_array(&encoder, &child1, len1);
        err2 = cbor_encoder_create_map(&encoder, &child2, len2);
        used1 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr1 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err3 = cbor_encoder_close_container(&encoder, &child1);
        err4 = cbor_encoder_close_container(&encoder, &child2);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err3;
        (void)err4;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    } else {
        len1 %= 4;

        err1 = cbor_encoder_create_array(&encoder, &child1, len1);
        used1 = cbor_encoder_get_buffer_size(&child1, buffer);
        extra1 = cbor_encoder_get_extra_bytes_needed(&child1);
        ptr1 = _cbor_encoder_get_buffer_pointer(&child1);
        (void)used1;
        (void)extra1;
        (void)ptr1;

        err2 = cbor_encoder_close_container(&encoder, &child1);
        err3 = cbor_encoder_close_container(&encoder, &child1);
        used2 = cbor_encoder_get_buffer_size(&encoder, buffer);
        extra2 = cbor_encoder_get_extra_bytes_needed(&encoder);
        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder);
        (void)err1;
        (void)err2;
        (void)err3;
        (void)used2;
        (void)extra2;
        (void)ptr2;
    }

    free(buffer);
    return 0;
}