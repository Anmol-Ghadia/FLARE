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
        CborValue next;
        size_t len = 0;
        CborError err;

        err = cbor_value_begin_string_iteration(&iter);
        if (err == CborNoError) {
            (void)cbor_value_is_byte_string(&iter);

            err = cbor_value_get_string_length(&iter, &len);
            if (err == CborNoError) {
                uint8_t stackbuf[4096];
                uint8_t *buf = stackbuf;
                size_t buflen = len;

                if (len > sizeof(stackbuf)) {
                    buf = (uint8_t *)malloc(len);
                    if (!buf)
                        goto tail_checks;
                }

                memset(&next, 0, sizeof(next));
                (void)cbor_value_copy_byte_string(&iter, buf, &buflen, &next);

                buflen = len;
                memset(&next, 0, sizeof(next));
                (void)cbor_value_copy_byte_string(&iter, buf, &buflen, &next);

                if (buf != stackbuf)
                    free(buf);
            }
        }

tail_checks:
        (void)cbor_value_at_end(&iter);
        (void)cbor_value_get_type(&iter);
        (void)cbor_value_get_string_length(&iter, &len);
        (void)cbor_value_at_end(&iter);
        (void)cbor_value_get_next_byte(&iter);
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