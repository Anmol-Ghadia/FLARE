// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_calculate_string_length at cborparser.c:954:11 in cbor.h
// _cbor_value_dup_string at cborparser_dup_string.c:96:11 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// _cbor_value_get_string_chunk_size at cborparser.c:1033:11 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// _cbor_value_get_string_chunk_size at cborparser.c:1033:11 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_finish_string_iteration at cbor.h:541:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_string_apis(const CborValue *value) {
    size_t len = 0;
    CborError err;

    if (!value)
        return;

    if (!(cbor_value_is_text_string(value) || cbor_value_is_byte_string(value)))
        return;

    err = cbor_value_get_string_length(value, &len);
    (void)err;

    len = 0;
    err = cbor_value_calculate_string_length(value, &len);
    (void)err;

    {
        void *buffer = NULL;
        size_t buflen = 0;
        CborValue next;
        err = _cbor_value_dup_string(value, &buffer, &buflen, &next);
        if (err == CborNoError)
            free(buffer);
    }

    {
        const void *chunk = NULL;
        size_t chunk_len = 0;
        CborValue next;
        err = _cbor_value_get_string_chunk(value, &chunk, &chunk_len, &next);
        (void)chunk;
        (void)chunk_len;
        (void)err;
    }

    {
        size_t chunk_len = 0;
        err = _cbor_value_get_string_chunk_size(value, &chunk_len);
        (void)err;
    }

    {
        CborValue iter = *value;
        err = cbor_value_begin_string_iteration(&iter);
        if (err == CborNoError) {
            int guard = 0;
            while (!cbor_value_string_iteration_at_end(&iter) && guard++ < 1024) {
                const void *chunk = NULL;
                size_t chunk_len = 0;
                CborValue next;
                CborError e1 = _cbor_value_get_string_chunk(&iter, &chunk, &chunk_len, &next);
                CborError e2 = _cbor_value_get_string_chunk_size(&iter, &chunk_len);
                (void)chunk;
                (void)e2;
                if (e1 != CborNoError)
                    break;
                iter = next;
            }

            if (cbor_value_string_iteration_at_end(&iter))
                cbor_value_finish_string_iteration(&iter);
        }
    }
}

static void walk_value(const CborValue *value, int depth) {
    CborValue recursed;
    CborError err;

    if (!value || depth > 32)
        return;

    if (cbor_value_is_text_string(value) || cbor_value_is_byte_string(value))
        exercise_string_apis(value);

    if (cbor_value_is_container(value)) {
        err = cbor_value_enter_container(value, &recursed);
        if (err == CborNoError) {
            int guard = 0;
            while (!cbor_value_at_end(&recursed) && guard++ < 1024) {
                walk_value(&recursed, depth + 1);
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size)
                fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        CborValue cur = it;
        int guard = 0;
        while (!cbor_value_at_end(&cur) && guard++ < 1024) {
            walk_value(&cur, 0);
            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}