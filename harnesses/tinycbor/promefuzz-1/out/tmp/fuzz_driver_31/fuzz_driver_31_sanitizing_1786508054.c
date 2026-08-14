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

static void fuzz_string_apis(CborValue *it) {
    CborValue next;
    size_t len = 0;
    CborError err;

    err = cbor_value_calculate_string_length(it, &len);
    (void)err;

    len = 0;
    err = _cbor_value_copy_string(it, NULL, &len, &next);
    (void)err;

    {
        size_t small_len = 0;
        char small_buf[8];
        memset(small_buf, 0, sizeof(small_buf));
        err = _cbor_value_copy_string(it, small_buf, &small_len, &next);
        (void)err;
    }

    {
        size_t exact_len = 0;
        err = _cbor_value_copy_string(it, NULL, &exact_len, NULL);
        if (exact_len < (1U << 20)) {
            char *buf = (char *)malloc(exact_len + 1);
            if (buf) {
                size_t buflen = exact_len;
                memset(buf, 0, exact_len + 1);
                err = _cbor_value_copy_string(it, buf, &buflen, &next);
                (void)err;
                free(buf);
            }
        }
    }

    {
        void *dup = NULL;
        size_t dup_len = 0;
        err = _cbor_value_dup_string(it, &dup, &dup_len, &next);
        if (dup) {
            free(dup);
        }
        (void)err;
    }

    {
        const void *chunk_ptr = NULL;
        size_t chunk_len = 0;
        CborValue chunk_next = *it;
        err = _cbor_value_get_string_chunk_size(it, &chunk_len);
        (void)err;
        err = _cbor_value_get_string_chunk(it, &chunk_ptr, &chunk_len, &chunk_next);
        (void)err;
    }

    if (cbor_value_is_length_known(it)) {
        size_t chunk_len = 0;
        err = cbor_value_get_string_chunk_size(it, &chunk_len);
        (void)err;
    }
}

static void walk_value(CborValue *it, int depth) {
    if (depth > 16)
        return;

    if (cbor_value_is_byte_string(it) || cbor_value_is_text_string(it)) {
        fuzz_string_apis(it);
    }

    if (cbor_value_is_array(it) || cbor_value_is_map(it)) {
        CborValue rec;
        CborError err = cbor_value_enter_container(it, &rec);
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 32) {
                walk_value(&rec, depth + 1);
                err = cbor_value_advance(&rec);
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

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    walk_value(&it, 0);

    {
        CborValue cur = it;
        int count = 0;
        while (!cbor_value_at_end(&cur) && count++ < 32) {
            if (cbor_value_is_byte_string(&cur) || cbor_value_is_text_string(&cur)) {
                fuzz_string_apis(&cur);
            }
            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}