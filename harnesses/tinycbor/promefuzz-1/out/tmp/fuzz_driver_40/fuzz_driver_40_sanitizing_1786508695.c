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

static void fuzz_string_apis(const CborValue *value) {
    size_t len = 0;
    size_t calc_len = SIZE_MAX;
    CborError err;
    CborValue next;

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

            iter = iter_next;
        }
    }
}

static void fuzz_container(CborValue *it, int depth) {
    if (depth > 8)
        return;

    while (!cbor_value_at_end(it)) {
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
                        CborError err = _cbor_value_get_string_chunk(&chunkit, &chunkptr, &chunklen, &next);
                        if (err != CborNoError)
                            break;
                        if (cbor_value_is_length_known(&chunkit) && t == CborTextStringType) {
                            size_t len2 = 0;
                            (void)cbor_value_get_string_chunk_size(&chunkit, &len2);
                        }
                        chunkit = next;
                    }
                }
            }
        } else if (t == CborArrayType || t == CborMapType) {
            CborValue rec;
            if (cbor_value_enter_container(it, &rec) == CborNoError) {
                fuzz_container(&rec, depth + 1);
                (void)cbor_value_leave_container(it, &rec);
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
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    fuzz_container(&it, 0);

    if (!cbor_value_at_end(&it)) {
        CborType t = cbor_value_get_type(&it);
        if (t == CborTextStringType || t == CborByteStringType) {
            if (cbor_value_is_length_known(&it)) {
                fuzz_string_apis(&it);
            } else if (t == CborTextStringType) {
                CborValue tmp = it;
                size_t len = 0;
                (void)cbor_value_get_string_chunk_size(&tmp, &len);
            }
        }
    }

    return 0;
}