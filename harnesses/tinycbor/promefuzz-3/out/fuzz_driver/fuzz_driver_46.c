// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_dup_byte_string at cbor.h:512:27 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_finish_string_iteration at cbor.h:541:27 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// cbor_value_string_iteration_at_end at cbor.h:526:22 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_finish_string_iteration at cbor.h:541:27 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static void consume_byte_string_apis(CborValue *it) {
    if (!it || !cbor_value_is_valid(it))
        return;
    if (!cbor_value_is_byte_string(it))
        return;

    {
        size_t len = 0;
        (void)cbor_value_get_string_length(it, &len);
    }

    {
        uint8_t stackbuf[256];
        size_t buflen = sizeof(stackbuf);
        CborValue next;
        (void)cbor_value_copy_byte_string(it, stackbuf, &buflen, &next);
    }

    {
        size_t buflen = 0;
        CborValue next;
        (void)cbor_value_copy_byte_string(it, NULL, &buflen, &next);
    }

    {
        uint8_t *dup = NULL;
        size_t dup_len = 0;
        CborValue next;
        if (cbor_value_dup_byte_string(it, &dup, &dup_len, &next) == CborNoError)
            free(dup);
    }

    {
        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            int complete = 1;
            while (!cbor_value_string_iteration_at_end(&iter)) {
                const uint8_t *chunk = NULL;
                size_t chunk_len = 0;
                CborValue next;
                CborError e = cbor_value_get_byte_string_chunk(&iter, &chunk, &chunk_len, &next);
                if (e != CborNoError) {
                    complete = 0;
                    break;
                }
                iter = next;
            }
            if (complete)
                (void)cbor_value_finish_string_iteration(&iter);
        }
    }

    {
        CborValue iter = *it;
        if (_cbor_value_begin_string_iteration(&iter) == CborNoError) {
            int complete = 1;
            while (!cbor_value_string_iteration_at_end(&iter)) {
                const uint8_t *chunk = NULL;
                size_t chunk_len = 0;
                CborValue next;
                CborError e = cbor_value_get_byte_string_chunk(&iter, &chunk, &chunk_len, &next);
                if (e != CborNoError) {
                    complete = 0;
                    break;
                }
                iter = next;
            }
            if (complete)
                (void)cbor_value_finish_string_iteration(&iter);
        }
    }
}

static void walk_value(const CborValue *it, int depth) {
    if (!it || !cbor_value_is_valid(it) || depth > 32)
        return;

    if (cbor_value_is_byte_string(it)) {
        CborValue tmp = *it;
        consume_byte_string_apis(&tmp);
        return;
    }

    if (cbor_value_is_array(it)) {
        CborValue rec;
        CborValue parent = *it;
        if (cbor_value_enter_container(&parent, &rec) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 64) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
                if (!cbor_value_is_valid(&rec))
                    break;
            }
        }
        return;
    }

    if (cbor_value_is_map(it)) {
        CborValue rec;
        CborValue parent = *it;
        if (cbor_value_enter_container(&parent, &rec) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 64) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
                if (!cbor_value_is_valid(&rec) || cbor_value_at_end(&rec))
                    break;

                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
                if (!cbor_value_is_valid(&rec))
                    break;
            }
        }
        return;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        CborParser parser;
        CborValue it;
        if (cbor_parser_init(Data, Size, 0, &parser, &it) == CborNoError)
            walk_value(&it, 0);
    }

    return 0;
}