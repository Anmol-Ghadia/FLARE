// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    memset(&parser, 0, sizeof(parser));
    parser.source.end = Data + Size;

    CborValue value;
    memset(&value, 0, sizeof(value));
    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = 0;
    value.extra = 0;
    value.flags = 0;

    if (Size > 0) {
        uint8_t major = Data[0] & 0xe0;
        switch (major) {
        case 0x40:
            value.type = CborByteStringType;
            break;
        case 0x60:
            value.type = CborTextStringType;
            break;
        case 0x80:
            value.type = CborArrayType;
            break;
        default:
            value.type = CborInvalidType;
            break;
        }
    } else {
        value.type = CborInvalidType;
    }

    (void)cbor_value_is_array(&value);

    bool is_bs = cbor_value_is_byte_string(&value);
    bool is_ts = cbor_value_is_text_string(&value);

    if (is_bs || is_ts) {
        CborValue iter = value;
        iter.flags = 0;
        (void)cbor_value_begin_string_iteration(&iter);
    }

    is_bs = cbor_value_is_byte_string(&value);

    if (is_bs) {
        uint8_t smallbuf[32];
        size_t smalllen = sizeof(smallbuf);
        CborValue next1;
        memset(&next1, 0, sizeof(next1));
        (void)cbor_value_copy_byte_string(&value, smallbuf, &smalllen, &next1);

        size_t dynlen = Size < 4096 ? Size : 4096;
        uint8_t *dynbuf = dynlen ? (uint8_t *)malloc(dynlen) : NULL;
        CborValue next2;
        memset(&next2, 0, sizeof(next2));
        (void)cbor_value_copy_byte_string(&value, dynbuf, &dynlen, &next2);
        free(dynbuf);
    }

    (void)cbor_value_at_end(&value);
    (void)cbor_value_get_type(&value);

    if (is_bs || is_ts) {
        size_t length = 0;
        (void)cbor_value_get_string_length(&value, &length);
    }

    (void)cbor_value_at_end(&value);
    (void)cbor_value_get_next_byte(&value);

    return 0;
}