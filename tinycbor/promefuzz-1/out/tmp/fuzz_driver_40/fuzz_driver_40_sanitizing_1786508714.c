#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void fuzz_string_apis(const CborValue *value) {
    size_t len = 0;
    size_t calc_len = SIZE_MAX;
    CborError err;

    err = cbor_value_calculate_string_length(value, &calc_len);
    (void)err;

    len = 0;
    err = _cbor_value_get_string_chunk_size(value, &len);
    (void)err;

    {
        const void *chunkptr = NULL;
        size_t chunklen = 0;
        CborValue chunknext;
        err = _cbor_value_get_string_chunk(value, &chunkptr, &chunklen, &chunknext);
        (void)err;
    }

    {
        size_t needed = 0;
        CborValue qnext;
        err = _cbor_value_copy_string(value, NULL, &needed, &qnext);
        (void)err;

        {
            size_t smallsz = needed > 0 ? needed - 1 : 0;
            char *smallbuf = NULL;
            CborValue snext;
            if (smallsz > 0)
                smallbuf = (char *)malloc(smallsz);
            err = _cbor_value_copy_string(value, smallbuf, &smallsz, &snext);
            free(smallbuf);
        }

        {
            size_t exactsz = needed;
            char *buf = NULL;
            CborValue cnext;
            if (exactsz > 0)
                buf = (char *)malloc(exactsz);
            err = _cbor_value_copy_string(value, buf, &exactsz, &cnext);
            free(buf);
        }

        {
            size_t largesz = needed < (SIZE_MAX - 8) ? needed + 8 : needed;
            char *buf = NULL;
            CborValue cnext;
            if (largesz > 0)
                buf = (char *)malloc(largesz);
            err = _cbor_value_copy_string(value, buf, &largesz, &cnext);
            free(buf);
        }
    }

    {
        void *dupbuf = NULL;
        size_t dupsz = 0;
        CborValue dnext;
        err = _cbor_value_dup_string(value, &dupbuf, &dupsz, &dnext);
        free(dupbuf);
    }

    {
        void *dupbuf = NULL;
        size_t dupsz = 1;
        err = _cbor_value_dup_string(value, &dupbuf, &dupsz, NULL);
        free(dupbuf);
    }

    {
        CborValue iter = *value;
        for (int i = 0; i < 32; ++i) {
            const void *chunkptr = NULL;
            size_t chunklen = 0;
            CborValue iter_next;
            err = _cbor_value_get_string_chunk(&iter, &chunkptr, &chunklen, &iter_next);
            if (err != CborNoError)
                break;

            {
                size_t clen = 0;
                (void)_cbor_value_get_string_chunk_size(&iter, &clen);
            }

            if (iter_next.type == CborInvalidType)
                break;
            iter = iter_next;
        }
    }
}

static void fuzz_value(CborValue *it, int depth);

static void fuzz_container(CborValue *container, int depth) {
    CborValue rec;
    CborError err;

    if (depth > 8)
        return;

    err = cbor_value_enter_container(container, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec)) {
        fuzz_value(&rec, depth + 1);

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(container, &rec);
}

static void fuzz_value(CborValue *it, int depth) {
    CborType t = cbor_value_get_type(it);

    if (t == CborTextStringType || t == CborByteStringType) {
        if (cbor_value_is_length_known(it)) {
            fuzz_string_apis(it);
        } else {
            CborValue rec = *it;
            fuzz_string_apis(&rec);

            if (t == CborTextStringType) {
                size_t len = 0;
                (void)cbor_value_get_string_chunk_size(&rec, &len);
            }

            {
                CborValue chunkit = *it;
                for (int i = 0; i < 32; ++i) {
                    const void *chunkptr = NULL;
                    size_t chunklen = 0;
                    CborValue next;
                    CborError err2 = _cbor_value_get_string_chunk(&chunkit, &chunkptr, &chunklen, &next);
                    if (err2 != CborNoError)
                        break;
                    if (chunkit.type == CborTextStringType) {
                        size_t len2 = 0;
                        (void)cbor_value_get_string_chunk_size(&chunkit, &len2);
                    }
                    if (next.type == CborInvalidType)
                        break;
                    chunkit = next;
                }
            }
        }
    } else if (t == CborArrayType || t == CborMapType) {
        fuzz_container(it, depth);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    if (!cbor_value_at_end(&it))
        fuzz_value(&it, 0);

    return 0;
}