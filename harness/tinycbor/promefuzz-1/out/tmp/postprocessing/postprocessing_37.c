#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static char *make_nul_terminated(const uint8_t *Data, size_t Size, size_t maxlen) {
    size_t n = Size < maxlen ? Size : maxlen;
    char *s = (char *)malloc(n + 1);
    if (!s)
        return NULL;
    if (n)
        memcpy(s, Data, n);
    s[n] = '\0';
    return s;
}

static void fuzz_value(const CborValue *value, const uint8_t *Data, size_t Size) {
    CborType type = cbor_value_get_type(value);

    {
        bool eq = false;
        char *cmp = make_nul_terminated(Data, Size, 64);
        if (cmp) {
            (void)cbor_value_text_string_equals(value, cmp, &eq);
            free(cmp);
        }
        (void)cbor_value_text_string_equals(value, "", &eq);
    }

    if (type == CborTextStringType || type == CborByteStringType) {
        const void *buf = NULL;
        size_t len = 0;
        CborValue next;
        (void)_cbor_value_get_string_chunk(value, &buf, &len, &next);
        (void)_cbor_value_get_string_chunk(value, &buf, &len, NULL);
    }

    if (type == CborTextStringType) {
        const char *buf = NULL;
        size_t len = 0;
        CborValue next;
        (void)cbor_value_get_text_string_chunk(value, &buf, &len, &next);
        (void)cbor_value_get_text_string_chunk(value, &buf, &len, NULL);

        {
            size_t outcap = Size > 128 ? 128 : Size;
            char *out = (char *)malloc(outcap + 1);
            if (out) {
                size_t buflen = outcap;
                CborValue next2;
                (void)cbor_value_copy_text_string(value, out, &buflen, &next2);
                buflen = 0;
                (void)cbor_value_copy_text_string(value, out, &buflen, NULL);
                free(out);
            }
        }
    }

    if (type == CborByteStringType) {
        const uint8_t *buf = NULL;
        size_t len = 0;
        CborValue next;
        (void)cbor_value_get_byte_string_chunk(value, &buf, &len, &next);
        (void)cbor_value_get_byte_string_chunk(value, &buf, &len, NULL);
    }

    if (type == CborMapType) {
        CborValue element;
        (void)cbor_value_map_find_value(value, "", &element);

        {
            char *key = make_nul_terminated(Data, Size, 64);
            if (key) {
                (void)cbor_value_map_find_value(value, key, &element);
                free(key);
            }
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
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    size_t steps = 0;

    while (!cbor_value_at_end(&cur) && steps++ < 64) {
        fuzz_value(&cur, Data, Size);

        {
            CborType type = cbor_value_get_type(&cur);
            if (type == CborArrayType || type == CborMapType) {
                CborValue rec;
                if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                    size_t inner_steps = 0;
                    while (!cbor_value_at_end(&rec) && inner_steps++ < 32) {
                        fuzz_value(&rec, Data, Size);
                        if (cbor_value_advance(&rec) != CborNoError)
                            break;
                    }
                    /* Do not call cbor_value_leave_container() after advancing rec;
                     * tinycbor requires rec.type == CborInvalidType there. */
                }
            }
        }

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}