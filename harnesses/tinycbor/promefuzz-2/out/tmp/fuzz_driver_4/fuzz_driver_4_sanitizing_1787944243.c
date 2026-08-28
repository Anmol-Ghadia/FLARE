#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = 0;
    value.extra = 0;
    value.flags = 0;

    if (Size > 0) {
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
            value.type = (uint8_t)Data[0];
            break;
        }
    } else {
        value.type = CborInvalidType;
    }

    if (Size > 1) {
        value.flags = Data[1];
    }

    if (Size > 2) {
        value.remaining = Data[2];
    }

    if (Size > 4) {
        value.extra = (uint16_t)((uint16_t)Data[3] << 8 | Data[4]);
    } else if (Size > 3) {
        value.extra = Data[3];
    }

    (void)cbor_value_get_type(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborArrayType) {
        size_t array_len = 0;
        (void)cbor_value_get_array_length(&value, &array_len);
    }

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborArrayType) {
        size_t array_len2 = 0;
        (void)cbor_value_get_array_length(&value, &array_len2);
    }

    (void)cbor_value_get_type(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborMapType) {
        size_t map_len = 0;
        (void)cbor_value_get_map_length(&value, &map_len);
    }

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborMapType) {
        size_t map_len2 = 0;
        (void)cbor_value_get_map_length(&value, &map_len2);
    }

    (void)cbor_value_is_text_string(&value);
    (void)cbor_value_is_byte_string(&value);

    (void)cbor_value_is_length_known(&value);

    if (value.type == CborTextStringType || value.type == CborByteStringType) {
        size_t str_len = 0;
        (void)cbor_value_get_string_length(&value, &str_len);

        size_t str_len2 = 0;
        (void)cbor_value_get_string_length(&value, &str_len2);
    }

    (void)cbor_value_get_next_byte(&value);

    return 0;
}