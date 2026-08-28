#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_byte_string_apis(CborValue *it) {
    CborValue next;
    size_t len = 0;
    CborError err;

    err = cbor_value_get_string_length(it, &len);
    if (err == CborNoError) {
        uint8_t *buf = NULL;
        size_t buflen;

        buflen = len;
        if (buflen > 0) {
            buf = (uint8_t *)malloc(buflen);
            if (buf) {
                CborValue tmpnext;
                size_t copylen = buflen;
                (void)cbor_value_copy_byte_string(it, buf, &copylen, &tmpnext);
                free(buf);
            }
        } else {
            uint8_t dummy = 0;
            CborValue tmpnext;
            size_t copylen = 0;
            (void)cbor_value_copy_byte_string(it, &dummy, &copylen, &tmpnext);
        }

        {
            uint8_t *dup = NULL;
            size_t duplen = 0;
            CborValue tmpnext;
            if (cbor_value_dup_byte_string(it, &dup, &duplen, &tmpnext) == CborNoError) {
                free(dup);
            }
        }

        {
            size_t small = len > 0 ? len - 1 : 0;
            uint8_t *smallbuf = NULL;
            if (small > 0) {
                smallbuf = (uint8_t *)malloc(small);
            }
            if (small == 0 || smallbuf) {
                CborValue tmpnext;
                size_t inout = small;
                uint8_t dummy = 0;
                (void)cbor_value_copy_byte_string(it, smallbuf ? smallbuf : &dummy, &inout, &tmpnext);
            }
            free(smallbuf);
        }
    }

    {
        CborValue chunkit = *it;
        if (cbor_value_begin_string_iteration(&chunkit) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const uint8_t *chunkptr = NULL;
                size_t chunklen = 0;
                CborValue nextchunk;
                CborError e = cbor_value_get_byte_string_chunk(&chunkit, &chunkptr, &chunklen, &nextchunk);
                if (e != CborNoError)
                    break;
                chunkit = nextchunk;
                if (cbor_value_string_iteration_at_end(&chunkit))
                    break;
            }
            (void)cbor_value_finish_string_iteration(&chunkit);
        }
    }

    (void)cbor_value_advance_fixed(it);
    (void)next;
}

static void traverse(CborValue *it, int depth) {
    if (depth > 16)
        return;

    while (!cbor_value_at_end(it)) {
        CborType type = cbor_value_get_type(it);

        if (type == CborByteStringType) {
            exercise_byte_string_apis(it);
            continue;
        }

        if (type == CborArrayType || type == CborMapType) {
            CborValue rec;
            if (cbor_value_enter_container(it, &rec) == CborNoError) {
                traverse(&rec, depth + 1);
                (void)cbor_value_leave_container(it, &rec);
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
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    traverse(&it, 0);

    if (cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it) == CborNoError) {
        traverse(&it, 0);
    }

    return 0;
}