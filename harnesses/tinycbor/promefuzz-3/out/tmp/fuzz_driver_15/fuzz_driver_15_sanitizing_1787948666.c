#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_string_apis(CborValue *value, size_t hint_size) {
    CborType t = cbor_value_get_type(value);
    if (t != CborTextStringType && t != CborByteStringType)
        return;

    {
        size_t calc_len = 0;
        (void)cbor_value_calculate_string_length(value, &calc_len);
    }

    {
        void *dupbuf = NULL;
        size_t dupsz = 0;
        CborValue next;
        (void)_cbor_value_dup_string(value, &dupbuf, &dupsz, &next);
        free(dupbuf);
    }

    {
        size_t cap = hint_size + 8;
        if (cap < 8)
            cap = 8;
        uint8_t *buf = (uint8_t *)malloc(cap);
        if (buf) {
            size_t buflen = cap;
            CborValue next;
            (void)_cbor_value_copy_string(value, buf, &buflen, &next);

            size_t smalllen = cap / 2;
            (void)_cbor_value_copy_string(value, buf, &smalllen, NULL);

            if (t == CborTextStringType) {
                size_t textlen = cap;
                CborValue textnext;
                (void)cbor_value_copy_text_string(value, (char *)buf, &textlen, &textnext);

                size_t textsmall = cap / 2;
                (void)cbor_value_copy_text_string(value, (char *)buf, &textsmall, NULL);
            }

            free(buf);
        }
    }

    {
        const void *chunkptr = NULL;
        size_t chunklen = 0;
        CborValue next;
        (void)_cbor_value_get_string_chunk(value, &chunkptr, &chunklen, &next);
        (void)_cbor_value_get_string_chunk(value, NULL, &chunklen, NULL);
    }

    if (cbor_value_is_length_known(value)) {
        size_t direct_len = 0;
        (void)cbor_value_get_string_length(value, &direct_len);
    }
}

static void walk_value(CborValue *value, size_t hint_size, int depth);

static void walk_container(CborValue *value, size_t hint_size, int depth) {
    CborValue rec;
    CborError err;
    size_t steps = 0;

    if (depth > 3)
        return;
    if (!cbor_value_is_container(value))
        return;

    err = cbor_value_enter_container(value, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec) && steps++ < 16) {
        walk_value(&rec, hint_size, depth + 1);

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;
    }
}

static void walk_value(CborValue *value, size_t hint_size, int depth) {
    exercise_string_apis(value, hint_size);

    if (depth > 3)
        return;

    if (cbor_value_is_container(value))
        walk_container(value, hint_size, depth);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    size_t steps = 0;
    while (!cbor_value_at_end(&it) && steps++ < 64) {
        walk_value(&it, Size, 0);

        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}