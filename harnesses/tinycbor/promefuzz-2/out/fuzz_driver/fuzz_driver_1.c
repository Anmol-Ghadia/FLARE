// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
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
        err = cbor_value_begin_string_iteration(&iter);
        if (err == CborNoError) {
            bool iter_is_bstr = cbor_value_is_byte_string(&iter);
            if (iter_is_bstr) {
                size_t known_len = 0;
                err = cbor_value_get_string_length(&iter, &known_len);
                if (err == CborNoError && known_len <= (1U << 20)) {
                    size_t buflen1 = known_len;
                    uint8_t *buf1 = NULL;

                    if (buflen1 == 0) {
                        uint8_t dummy = 0;
                        CborValue next1;
                        memset(&next1, 0, sizeof(next1));
                        (void)cbor_value_copy_byte_string(&iter, &dummy, &buflen1, &next1);
                    } else {
                        buf1 = (uint8_t *)malloc(buflen1);
                        if (buf1) {
                            CborValue next1;
                            memset(&next1, 0, sizeof(next1));
                            (void)cbor_value_copy_byte_string(&iter, buf1, &buflen1, &next1);

                            size_t buflen2 = buflen1;
                            uint8_t *buf2 = (uint8_t *)malloc(buflen2 ? buflen2 : 1);
                            if (buf2) {
                                CborValue next2;
                                memset(&next2, 0, sizeof(next2));
                                (void)cbor_value_copy_byte_string(&iter, buf2, &buflen2, &next2);
                                free(buf2);
                            }

                            free(buf1);
                        }
                    }
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

            size_t slen = 0;
            (void)cbor_value_get_string_length(&value, &slen);

            (void)cbor_value_at_end(&value);
            (void)cbor_value_get_next_byte(&value);
        }
    } else {
        (void)cbor_value_at_end(&value);
        (void)cbor_value_get_type(&value);

        size_t slen = 0;
        if (is_bstr || is_tstr) {
            (void)cbor_value_get_string_length(&value, &slen);
        }

        (void)cbor_value_at_end(&value);
        (void)cbor_value_get_next_byte(&value);
    }

    return 0;
}