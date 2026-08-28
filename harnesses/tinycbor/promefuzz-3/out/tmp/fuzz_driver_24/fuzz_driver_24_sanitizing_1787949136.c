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

static void exercise_text_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    bool eq = false;
    char *buffer = NULL;
    size_t buflen = 0;
    CborValue next = *value;

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

        for (int i = 0; i < 32; ++i) {
            err = cbor_value_get_text_string_chunk(&iter, &chunk_ptr, &chunk_len, &iter);
            if (err != CborNoError)
                break;
            (void)chunk_ptr;
            (void)chunk_len;
        }
        (void)_cbor_value_finish_string_iteration(&iter);
    }
}

static void exercise_map_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    CborValue found;
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

    {
        CborValue reparsed = it;
        (void)cbor_value_reparse(&reparsed);
    }

    if (cbor_value_is_text_string(&it))
        exercise_text_value(&it, Data, Size);

    if (cbor_value_is_map(&it))
        exercise_map_value(&it, Data, Size);

    {
        CborValue cursor = it;
        for (int depth = 0; depth < 16; ++depth) {
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
                cursor = child;
                continue;
            }

            break;
        }
    }

    return 0;
}