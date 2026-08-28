// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_array_length at cbor.h:570:27 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_array_length at cbor.h:570:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_map_length at cbor.h:583:27 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_map_length at cbor.h:583:27 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    size_t tmp_len = 0;
    const uint8_t *next_byte;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = 0;
    value.flags = 0;
    value.extra = 0;

    if (Size == 0) {
        value.type = CborInvalidType;
    } else {
        switch (Data[0] % 5) {
        case 0:
            value.type = CborArrayType;
            break;
        case 1:
            value.type = CborMapType;
            break;
        case 2:
            value.type = CborTextStringType;
            break;
        case 3:
            value.type = CborByteStringType;
            break;
        default:
            value.type = CborInvalidType;
            break;
        }

        /*
         * Only keep "safe" flags that won't cause helper accessors to decode
         * additional bytes from source.ptr. In particular, do not allow
         * CborIteratorFlag_IntegerValueTooLarge / IntegerValueIs64Bit to be
         * set on a fabricated CborValue.
         */
        if (Size > 1)
            value.flags = Data[1] & (uint8_t)CborIteratorFlag_UnknownLength;
        else
            value.flags = 0;

        /*
         * Keep extra in the small/immediate range so accessor helpers won't
         * attempt to decode additional bytes from source.ptr.
         */
        if (Size > 2)
            value.extra = (uint16_t)(Data[2] % 24);
        else
            value.extra = 0;
    }

    (void)cbor_value_get_type(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborArrayType) {
        (void)cbor_value_get_array_length(&value, &tmp_len);
    }

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborArrayType) {
        (void)cbor_value_get_array_length(&value, &tmp_len);
    }

    (void)cbor_value_get_type(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborMapType) {
        (void)cbor_value_get_map_length(&value, &tmp_len);
    }

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborMapType) {
        (void)cbor_value_get_map_length(&value, &tmp_len);
    }

    (void)cbor_value_is_text_string(&value);
    (void)cbor_value_is_byte_string(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborTextStringType || value.type == CborByteStringType) {
        (void)cbor_value_get_string_length(&value, &tmp_len);
        (void)cbor_value_get_string_length(&value, &tmp_len);
    }

    next_byte = cbor_value_get_next_byte(&value);
    (void)next_byte;

    return 0;
}