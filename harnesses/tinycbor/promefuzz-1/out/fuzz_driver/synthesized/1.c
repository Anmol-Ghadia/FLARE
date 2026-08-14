// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_1(const uint8_t *Data, size_t Size) {
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