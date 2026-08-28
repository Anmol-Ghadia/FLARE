// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_dup_text_string at cbor.h:506:27 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
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

static int value_ptr_in_bounds(const CborValue *value) {
    if (!value || !value->parser)
        return 0;
    if (!value->source.ptr)
        return 0;
    if (!value->parser->source.end)
        return 0;
    return value->source.ptr < value->parser->source.end;
}

static void exercise_text_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    bool eq = false;
    char *buffer = NULL;
    size_t buflen = 0;
    CborValue next;

    if (!value_ptr_in_bounds(value))
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

        if (!value_ptr_in_bounds(&iter))
            return;

        for (int i = 0; i < 32; ++i) {
            err = cbor_value_get_text_string_chunk(&iter, &chunk_ptr, &chunk_len, &iter);
            if (err != CborNoError)
                break;
            if (!value_ptr_in_bounds(&iter))
                break;
            (void)chunk_ptr;
            (void)chunk_len;
        }
        if (value_ptr_in_bounds(&iter))
            (void)_cbor_value_finish_string_iteration(&iter);
    }
}

static void exercise_map_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    CborValue found;

    if (!value_ptr_in_bounds(value))
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

int LLVMFuzzerTestOneInput_24(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    write_dummy_file(Data, Size);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (!value_ptr_in_bounds(&it))
        return 0;

    {
        CborValue reparsed = it;
        if (value_ptr_in_bounds(&reparsed))
            (void)cbor_value_reparse(&reparsed);
    }

    if (cbor_value_is_text_string(&it))
        exercise_text_value(&it, Data, Size);

    if (cbor_value_is_map(&it))
        exercise_map_value(&it, Data, Size);

    return 0;
}