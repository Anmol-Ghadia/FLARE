#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError) {
        return 0;
    }

    (void)cbor_value_is_array(&it);
    (void)cbor_value_is_byte_string(&it);
    (void)cbor_value_is_text_string(&it);

    if (cbor_value_is_byte_string(&it)) {
        CborValue iter = it;
        uint8_t smallbuf[4096];
        size_t buflen;
        CborValue next;

        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            (void)cbor_value_is_byte_string(&iter);

            buflen = sizeof(smallbuf);
            memset(&next, 0, sizeof(next));
            (void)cbor_value_copy_byte_string(&iter, smallbuf, &buflen, &next);

            buflen = 0;
            memset(&next, 0, sizeof(next));
            (void)cbor_value_copy_byte_string(&iter, smallbuf, &buflen, &next);

            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_type(&iter);
            (void)cbor_value_get_string_length(&iter, &buflen);
            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_next_byte(&iter);
        }
    } else if (cbor_value_is_text_string(&it)) {
        CborValue iter = it;
        size_t slen = 0;

        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            (void)cbor_value_is_byte_string(&iter);
            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_type(&iter);
            (void)cbor_value_get_string_length(&iter, &slen);
            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_next_byte(&iter);
        }
    } else {
        (void)cbor_value_at_end(&it);
        (void)cbor_value_get_type(&it);
        (void)cbor_value_at_end(&it);
        (void)cbor_value_get_next_byte(&it);
    }

    return 0;
}