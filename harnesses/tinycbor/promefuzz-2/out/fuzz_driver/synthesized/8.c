// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL || Size == 0)
        return 0;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborType type = cbor_value_get_type(&it);

    if (type == CborByteStringType) {
        const uint8_t *buf = NULL;
        size_t len = 0;
        CborValue next;
        memset(&next, 0, sizeof(next));

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_byte_string_chunk(&it, &buf, &len, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_byte_string_chunk(&it, NULL, &len, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_byte_string_chunk(&it, &buf, NULL, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_byte_string_chunk(&it, &buf, &len, NULL);
    } else if (type == CborTextStringType) {
        const char *buf = NULL;
        size_t len = 0;
        CborValue next;
        memset(&next, 0, sizeof(next));

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_text_string_chunk(&it, &buf, &len, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_text_string_chunk(&it, NULL, &len, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_text_string_chunk(&it, &buf, NULL, &next);

        (void)cbor_value_get_type(&it);
        (void)cbor_value_get_text_string_chunk(&it, &buf, &len, NULL);
    }

    return 0;
}