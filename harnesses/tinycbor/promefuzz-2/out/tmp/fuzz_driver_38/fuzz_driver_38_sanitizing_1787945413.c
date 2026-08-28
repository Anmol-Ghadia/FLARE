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

static void fuzz_string_value(CborValue *it) {
    if (!it)
        return;

    if (!cbor_value_is_text_string(it))
        return;

    size_t len = 0;
    (void)cbor_value_get_string_length(it, &len);

    char *dup = NULL;
    size_t dup_len = 0;
    CborValue next_from_dup = *it;
    if (cbor_value_dup_text_string(it, &dup, &dup_len, &next_from_dup) == CborNoError) {
        free(dup);
    }

    CborValue iter = *it;
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
        if (cbor_value_string_iteration_at_end(&iter)) {
            (void)cbor_value_finish_string_iteration(&iter);
        }
    }

    iter = *it;
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
        if (cbor_value_string_iteration_at_end(&iter)) {
            (void)cbor_value_finish_string_iteration(&iter);
        }
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 16)
        return;

    if (cbor_value_is_text_string(it)) {
        fuzz_string_value(it);
        return;
    }

    if (cbor_value_is_array(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int guard = 0;
            while (!cbor_value_at_end(&rec) && guard++ < 256) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
        return;
    }

    if (cbor_value_is_map(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int guard = 0;
            while (!cbor_value_at_end(&rec) && guard++ < 256) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
                if (cbor_value_at_end(&rec))
                    break;
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
        return;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
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