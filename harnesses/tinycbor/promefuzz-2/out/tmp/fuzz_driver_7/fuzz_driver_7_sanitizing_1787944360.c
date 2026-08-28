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

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)(Size & 0xffffffffu);

    if (Size > 0) {
        value.type = Data[0];
    } else {
        value.type = CborInvalidType;
    }

    if (Size > 1) {
        value.flags = Data[1];
    }
    if (Size > 3) {
        value.extra = (uint16_t)((uint16_t)Data[2] | ((uint16_t)Data[3] << 8));
    }

    if (Size > 4) {
        parser.source.end = Data + Size;
    } else {
        parser.source.end = Data;
    }

    (void)cbor_value_is_array(&value);
    (void)cbor_value_is_map(&value);
    (void)cbor_value_is_valid(&value);

    {
        CborValue v = value;
        v.type = CborArrayType;
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    {
        CborValue v = value;
        v.type = CborMapType;
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    {
        CborValue v = value;
        v.type = CborInvalidType;
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    {
        CborValue v = value;
        v.type = (uint8_t)(Size > 5 ? Data[5] : 0xff);
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    (void)cbor_value_is_array(NULL);
    (void)cbor_value_is_map(NULL);
    (void)cbor_value_is_valid(NULL);

    return 0;
}