#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborError err;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    err = cbor_parser_init(Data, Size, 0, &parser, &value);
    if (err != CborNoError) {
        return 0;
    }

    (void)cbor_value_is_array(&value);
    bool is_bstr = cbor_value_is_byte_string(&value);
    bool is_tstr = cbor_value_is_text_string(&value);

    if (is_bstr || is_tstr) {
        CborValue iter = value;
        (void)cbor_value_begin_string_iteration(&iter);

        bool iter_is_bstr = cbor_value_is_byte_string(&iter);
        if (iter_is_bstr) {
            size_t buflen1 = 0;
            CborValue next1;
            memset(&next1, 0, sizeof(next1));
            (void)cbor_value_copy_byte_string(&iter, NULL, &buflen1, &next1);

            if (buflen1 > 0 && buflen1 < (1U << 20)) {
                uint8_t *buf = (uint8_t *)malloc(buflen1);
                if (buf) {
                    size_t buflen2 = buflen1;
                    CborValue next2;
                    memset(&next2, 0, sizeof(next2));
                    (void)cbor_value_copy_byte_string(&iter, buf, &buflen2, &next2);
                    free(buf);
                }
            } else {
                size_t smalllen = 1;
                uint8_t smallbuf[1];
                CborValue next3;
                memset(&next3, 0, sizeof(next3));
                (void)cbor_value_copy_byte_string(&iter, smallbuf, &smalllen, &next3);
            }
        }

        (void)cbor_value_at_end(&iter);
        (void)cbor_value_get_type(&iter);

        size_t slen = 0;
        (void)cbor_value_get_string_length(&iter, &slen);

        (void)cbor_value_at_end(&iter);
        (void)cbor_value_get_next_byte(&iter);
    } else {
        (void)cbor_value_at_end(&value);
        (void)cbor_value_get_type(&value);

        if (cbor_value_is_byte_string(&value) || cbor_value_is_text_string(&value)) {
            size_t slen = 0;
            (void)cbor_value_get_string_length(&value, &slen);
        }

        (void)cbor_value_at_end(&value);
        (void)cbor_value_get_next_byte(&value);
    }

    return 0;
}