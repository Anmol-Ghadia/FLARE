// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_dup_text_string at cbor.h:506:27 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_finish_string_iteration at cbor.h:541:27 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_finish_string_iteration at cbor.h:541:27 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "cbor.h"

static void fuzz_text_string(CborValue *value) {
    size_t len = 0;
    (void)cbor_value_get_string_length(value, &len);

    char *buffer = NULL;
    size_t buflen = 0;
    CborValue next = *value;
    if (cbor_value_dup_text_string(value, &buffer, &buflen, &next) == CborNoError) {
        free(buffer);
    }

    CborValue iter = *value;
    if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
        int guard = 0;
        while (!cbor_value_string_iteration_at_end(&iter) && guard++ < 1024) {
            const char *chunk = NULL;
            size_t chunk_len = 0;
            CborValue next_chunk = iter;
            CborError err = cbor_value_get_text_string_chunk(&iter, &chunk, &chunk_len, &next_chunk);
            if (err != CborNoError)
                break;
            iter = next_chunk;
        }
        if (cbor_value_string_iteration_at_end(&iter))
            (void)cbor_value_finish_string_iteration(&iter);
    }

    iter = *value;
    if (_cbor_value_begin_string_iteration(&iter) == CborNoError) {
        int guard = 0;
        while (!cbor_value_string_iteration_at_end(&iter) && guard++ < 1024) {
            const char *chunk = NULL;
            size_t chunk_len = 0;
            CborValue next_chunk = iter;
            CborError err = cbor_value_get_text_string_chunk(&iter, &chunk, &chunk_len, &next_chunk);
            if (err != CborNoError)
                break;
            iter = next_chunk;
        }
        if (cbor_value_string_iteration_at_end(&iter))
            (void)cbor_value_finish_string_iteration(&iter);
    }
}

static void walk_value(CborValue *value, int depth) {
    if (!value || depth > 16)
        return;

    if (cbor_value_is_text_string(value)) {
        fuzz_text_string(value);
        return;
    }

    if (cbor_value_is_array(value) || cbor_value_is_map(value)) {
        CborValue rec;
        if (cbor_value_enter_container(value, &rec) != CborNoError)
            return;

        int guard = 0;
        while (!cbor_value_at_end(&rec) && guard++ < 256) {
            walk_value(&rec, depth + 1);
            if (cbor_value_advance(&rec) != CborNoError)
                break;
        }
    }
}

int LLVMFuzzerTestOneInput_38(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    CborValue cur = it;
    int guard = 0;
    while (!cbor_value_at_end(&cur) && guard++ < 256) {
        walk_value(&cur, 0);
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}