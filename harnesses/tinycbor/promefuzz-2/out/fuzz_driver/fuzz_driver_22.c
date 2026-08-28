// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encoder_get_extra_bytes_needed at cbor.h:285:24 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_uint at cborencoder.c:348:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static size_t consume_size_t(const uint8_t **p, size_t *n) {
    size_t v = 0;
    size_t take = *n < sizeof(size_t) ? *n : sizeof(size_t);
    for (size_t i = 0; i < take; ++i)
        v = (v << 8) | (*p)[i];
    *p += take;
    *n -= take;
    return v;
}

static int consume_bool(const uint8_t **p, size_t *n) {
    if (*n == 0)
        return 0;
    int v = (**p) & 1;
    (*p)++;
    (*n)--;
    return v;
}

static CborError maybe_add_items(CborEncoder *enc, const uint8_t **p, size_t *n, size_t count) {
    CborError err = CborNoError;
    for (size_t i = 0; i < count && *n > 0; ++i) {
        uint8_t b = **p;
        (*p)++;
        (*n)--;

        switch (b % 6) {
        case 0:
            err = cbor_encode_uint(enc, b);
            break;
        case 1:
            err = cbor_encode_int(enc, (int8_t)b);
            break;
        case 2:
            err = cbor_encode_boolean(enc, b & 1);
            break;
        case 3:
            err = cbor_encode_null(enc);
            break;
        case 4:
            err = cbor_encode_simple_value(enc, b);
            break;
        default:
            err = cbor_encode_text_stringz(enc, "x");
            break;
        }

        if (err != CborNoError && err != CborErrorOutOfMemory)
            break;
    }
    return err;
}

static CborError maybe_add_map_pairs(CborEncoder *enc, const uint8_t **p, size_t *n, size_t pairs) {
    CborError err = CborNoError;
    for (size_t i = 0; i < pairs && *n > 0; ++i) {
        uint8_t keyb = **p;
        (*p)++;
        (*n)--;

        if (keyb & 1)
            err = cbor_encode_text_stringz(enc, "k");
        else
            err = cbor_encode_uint(enc, keyb);

        if (err != CborNoError && err != CborErrorOutOfMemory)
            return err;

        if (*n == 0)
            break;

        uint8_t valb = **p;
        (*p)++;
        (*n)--;

        switch (valb % 5) {
        case 0:
            err = cbor_encode_uint(enc, valb);
            break;
        case 1:
            err = cbor_encode_int(enc, (int8_t)valb);
            break;
        case 2:
            err = cbor_encode_boolean(enc, valb & 1);
            break;
        case 3:
            err = cbor_encode_null(enc);
            break;
        default:
            err = cbor_encode_text_stringz(enc, "v");
            break;
        }

        if (err != CborNoError && err != CborErrorOutOfMemory)
            return err;
    }
    return err;
}

static void fuzz_with_buffer(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    size_t buf_size = n ? (size_t)(*p % (n + 1)) : 0;
    if (n > 0) {
        p++;
        n--;
    }

    uint8_t *buffer = NULL;
    if (buf_size > 0) {
        buffer = (uint8_t *)malloc(buf_size);
        if (!buffer)
            return;
        memset(buffer, 0, buf_size);
    }

    CborEncoder root;
    cbor_encoder_init(&root, buffer, buf_size, 0);

    CborEncoder child;
    memset(&child, 0, sizeof(child));

    size_t length = consume_size_t(&p, &n);
    if (consume_bool(&p, &n))
        length = CborIndefiniteLength;
    else if (consume_bool(&p, &n))
        length %= 8;

    CborError err;
    if (consume_bool(&p, &n)) {
        err = cbor_encoder_create_array(&root, &child, length);
        if (err == CborNoError || err == CborErrorOutOfMemory) {
            size_t items = consume_size_t(&p, &n) % 12;
            (void)maybe_add_items(&child, &p, &n, items);

            if (consume_bool(&p, &n))
                (void)cbor_encoder_close_container(&root, &child);
            else
                (void)cbor_encoder_close_container_checked(&root, &child);
        }
    } else {
        err = cbor_encoder_create_map(&root, &child, length);
        if (err == CborNoError || err == CborErrorOutOfMemory) {
            size_t pairs = consume_size_t(&p, &n) % 8;
            (void)maybe_add_map_pairs(&child, &p, &n, pairs);

            if (consume_bool(&p, &n))
                (void)cbor_encoder_close_container(&root, &child);
            else
                (void)cbor_encoder_close_container_checked(&root, &child);
        }
    }

    (void)cbor_encoder_get_extra_bytes_needed(&root);
    (void)cbor_encoder_get_extra_bytes_needed(&child);

    free(buffer);
}

static void fuzz_with_writer(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    /*
     * Avoid invoking container creation/encoding in writer mode with a NULL
     * callback, which can dereference a null function pointer inside tinycbor.
     * Still exercise the target init API and query API safely.
     */
    CborEncoder root;
    cbor_encoder_init_writer(&root, NULL, NULL);
    (void)cbor_encoder_get_extra_bytes_needed(&root);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    fuzz_with_buffer(Data, Size);
    fuzz_with_writer(Data, Size);
    return 0;
}