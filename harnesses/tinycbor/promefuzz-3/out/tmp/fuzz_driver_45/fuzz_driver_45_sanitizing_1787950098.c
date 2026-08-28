#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_byte_string_apis(CborValue *it) {
    size_t len = 0;
    CborError err = cbor_value_get_string_length(it, &len);

    if (err == CborNoError) {
        uint8_t *buf = NULL;
        size_t buflen = len;

        if (buflen > 0) {
            buf = (uint8_t *)malloc(buflen);
            if (buf != NULL) {
                CborValue next;
                size_t copylen = buflen;
                (void)cbor_value_copy_byte_string(it, buf, &copylen, &next);
                free(buf);
            }
        } else {
            uint8_t dummy = 0;
            CborValue next;
            size_t copylen = 0;
            (void)cbor_value_copy_byte_string(it, &dummy, &copylen, &next);
        }

        {
            uint8_t *dup = NULL;
            size_t duplen = 0;
            CborValue next;
            if (cbor_value_dup_byte_string(it, &dup, &duplen, &next) == CborNoError) {
                free(dup);
            }
        }

        {
            size_t small = (len > 0) ? (len - 1) : 0;
            uint8_t *smallbuf = NULL;
            if (small > 0)
                smallbuf = (uint8_t *)malloc(small);

            if (small == 0 || smallbuf != NULL) {
                uint8_t dummy = 0;
                CborValue next;
                size_t inout = small;
                (void)cbor_value_copy_byte_string(it, smallbuf ? smallbuf : &dummy, &inout, &next);
            }
            free(smallbuf);
        }
    }

    {
        CborValue chunkit = *it;
        if (cbor_value_begin_string_iteration(&chunkit) == CborNoError) {
            int steps = 0;
            while (!cbor_value_string_iteration_at_end(&chunkit) && steps++ < 64) {
                const uint8_t *chunkptr = NULL;
                size_t chunklen = 0;
                CborValue nextchunk;
                CborError e = cbor_value_get_byte_string_chunk(&chunkit, &chunkptr, &chunklen, &nextchunk);
                if (e != CborNoError)
                    break;
                chunkit = nextchunk;
            }
            (void)cbor_value_finish_string_iteration(&chunkit);
        }
    }
}

static void traverse(CborValue *it, int depth) {
    if (depth > 16)
        return;

    while (!cbor_value_at_end(it)) {
        CborType type = cbor_value_get_type(it);

        if (type == CborByteStringType) {
            CborValue current = *it;
            exercise_byte_string_apis(&current);
            if (cbor_value_advance(it) != CborNoError)
                break;
            continue;
        }

        if (type == CborArrayType || type == CborMapType) {
            CborValue rec;
            if (cbor_value_enter_container(it, &rec) == CborNoError) {
                traverse(&rec, depth + 1);
                if (cbor_value_leave_container(it, &rec) != CborNoError)
                    break;
                continue;
            }
        }

        if (cbor_value_advance(it) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) == CborNoError)
        traverse(&it, 0);

    if (cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it) == CborNoError)
        traverse(&it, 0);

    return 0;
}