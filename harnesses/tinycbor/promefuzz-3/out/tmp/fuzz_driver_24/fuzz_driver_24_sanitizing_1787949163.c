#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (size)
        fwrite(data, 1, size, fp);
    fclose(fp);
}

static int has_min_bytes(const CborValue *value, size_t n) {
    if (!value || !value->parser)
        return 0;
    if (!value->source.ptr)
        return 0;
    return (size_t)(value->parser->source.end - value->source.ptr) >= n;
}

static void exercise_text_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    bool eq = false;
    char *buffer = NULL;
    size_t buflen = 0;
    CborValue next;

    if (!has_min_bytes(value, 1))
        return;

    next = *value;
    (void)cbor_value_text_string_equals(value, "", &eq);
    (void)cbor_value_dup_text_string(value, &buffer, &buflen, &next);
    free(buffer);

    if (Size > 0) {
        size_t n = Size > 32 ? 32 : Size;
        char *cmp = (char *)malloc(n + 1);
        if (cmp) {
            memcpy(cmp, Data, n);
            cmp[n] = '\0';
            (void)cbor_value_text_string_equals(value, cmp, &eq);
            free(cmp);
        }
    }

    if (!cbor_value_is_length_known(value)) {
        CborValue iter = *value;
        size_t chunk_len = 0;
        const void *chunk_ptr = NULL;
        CborError err;

        if (!has_min_bytes(&iter, 1))
            return;

        for (int i = 0; i < 32; ++i) {
            err = cbor_value_get_text_string_chunk(&iter, &chunk_ptr, &chunk_len, &iter);
            if (err != CborNoError)
                break;
            if (!has_min_bytes(&iter, 1))
                break;
            (void)chunk_ptr;
            (void)chunk_len;
        }
        if (has_min_bytes(&iter, 1))
            (void)_cbor_value_finish_string_iteration(&iter);
    }
}

static void exercise_map_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    CborValue found;

    if (!has_min_bytes(value, 1))
        return;

    (void)cbor_value_map_find_value(value, "", &found);
    (void)cbor_value_map_find_value(value, "a", &found);

    if (Size > 0) {
        size_t n = Size > 32 ? 32 : Size;
        char *key = (char *)malloc(n + 1);
        if (key) {
            memcpy(key, Data, n);
            key[n] = '\0';
            (void)cbor_value_map_find_value(value, key, &found);
            free(key);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    write_dummy_file(Data, Size);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (!has_min_bytes(&it, 1))
        return 0;

    {
        CborValue reparsed = it;
        if (has_min_bytes(&reparsed, 1))
            (void)cbor_value_reparse(&reparsed);
    }

    if (cbor_value_is_text_string(&it))
        exercise_text_value(&it, Data, Size);

    if (cbor_value_is_map(&it))
        exercise_map_value(&it, Data, Size);

    {
        CborValue cursor = it;
        for (int depth = 0; depth < 16; ++depth) {
            if (!has_min_bytes(&cursor, 1))
                break;

            (void)cbor_value_reparse(&cursor);

            if (cbor_value_is_text_string(&cursor)) {
                exercise_text_value(&cursor, Data, Size);
                break;
            }

            if (cbor_value_is_map(&cursor)) {
                exercise_map_value(&cursor, Data, Size);
                break;
            }

            if (cbor_value_is_container(&cursor)) {
                CborValue child;
                if (cbor_value_enter_container(&cursor, &child) != CborNoError)
                    break;
                if (!has_min_bytes(&child, 1))
                    break;
                cursor = child;
                continue;
            }

            break;
        }
    }

    return 0;
}