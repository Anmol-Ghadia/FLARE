#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_string_apis(CborValue *value, size_t hint_size) {
    CborType t = cbor_value_get_type(value);
    if (t != CborTextStringType && t != CborByteStringType)
        return;

    {
        size_t calc_len = 0;
        (void)cbor_value_calculate_string_length(value, &calc_len);
    }

    {
        void *dupbuf = NULL;
        size_t dupsz = 0;
        CborValue next;
        (void)_cbor_value_dup_string(value, &dupbuf, &dupsz, &next);
        free(dupbuf);
    }

    {
        size_t cap = hint_size + 8;
        if (cap < 8)
            cap = 8;
        uint8_t *buf = (uint8_t *)malloc(cap);
        if (buf) {
            size_t buflen = cap;
            CborValue next;
            (void)_cbor_value_copy_string(value, buf, &buflen, &next);

            size_t smalllen = cap / 2;
            (void)_cbor_value_copy_string(value, buf, &smalllen, NULL);

            if (t == CborTextStringType) {
                size_t textlen = cap;
                CborValue textnext;
                (void)cbor_value_copy_text_string(value, (char *)buf, &textlen, &textnext);

                size_t textsmall = cap / 2;
                (void)cbor_value_copy_text_string(value, (char *)buf, &textsmall, NULL);
            }

            free(buf);
        }
    }

    {
        const void *chunkptr = NULL;
        size_t chunklen = 0;
        CborValue next;
        (void)_cbor_value_get_string_chunk(value, &chunkptr, &chunklen, &next);
        (void)_cbor_value_get_string_chunk(value, NULL, &chunklen, NULL);
    }

    if (cbor_value_is_length_known(value)) {
        size_t direct_len = 0;
        (void)cbor_value_get_string_length(value, &direct_len);
    } else {
        CborValue recursed = *value;
        size_t inner_steps = 0;
        while (!cbor_value_at_end(&recursed) && inner_steps++ < 32) {
            size_t chunk_size = 0;
            (void)cbor_value_get_string_chunk_size(&recursed, &chunk_size);

            const void *chunkptr = NULL;
            size_t chunklen = 0;
            CborValue nextchunk;
            (void)_cbor_value_get_string_chunk(&recursed, &chunkptr, &chunklen, &nextchunk);

            CborError err = cbor_value_advance(&recursed);
            if (err != CborNoError)
                break;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue cur;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &cur);
    if (err != CborNoError)
        return 0;

    size_t steps = 0;
    while (!cbor_value_at_end(&cur) && steps++ < 64) {
        exercise_string_apis(&cur, Size);

        if (cbor_value_is_container(&cur)) {
            CborValue rec;
            err = cbor_value_enter_container(&cur, &rec);
            if (err == CborNoError) {
                size_t inner = 0;
                while (!cbor_value_at_end(&rec) && inner++ < 8) {
                    exercise_string_apis(&rec, Size);

                    if (cbor_value_is_container(&rec)) {
                        CborValue nested;
                        CborError nerr = cbor_value_enter_container(&rec, &nested);
                        if (nerr == CborNoError) {
                            size_t nested_steps = 0;
                            while (!cbor_value_at_end(&nested) && nested_steps++ < 4) {
                                exercise_string_apis(&nested, Size);
                                nerr = cbor_value_advance(&nested);
                                if (nerr != CborNoError)
                                    break;
                            }
                            (void)cbor_value_leave_container(&rec, &nested);
                        }
                    }

                    err = cbor_value_advance(&rec);
                    if (err != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&cur, &rec);
            }
        }

        CborValue next = cur;
        err = cbor_value_advance(&next);
        if (err != CborNoError)
            break;
        cur = next;
    }

    return 0;
}