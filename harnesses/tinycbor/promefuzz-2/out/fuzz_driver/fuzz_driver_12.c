// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_is_null at cbor.h:385:22 in cbor.h
// cbor_value_is_undefined at cbor.h:387:22 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_get_simple_type at cbor.h:403:27 in cbor.h
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

static void exercise_value(CborValue *value) {
    uint8_t simple = 0;

    (void)cbor_value_is_simple_type(value);
    (void)cbor_value_is_null(value);
    (void)cbor_value_is_undefined(value);
    (void)cbor_value_is_integer(value);
    (void)cbor_value_is_tag(value);

    if (cbor_value_is_simple_type(value)) {
        (void)cbor_value_get_simple_type(value, &simple);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    uint8_t storage[32];
    size_t copy_len = Size < sizeof(storage) ? Size : sizeof(storage);
    uint16_t extra = 0;
    uint32_t remaining = 0;
    uint8_t flags = 0;
    uint8_t selector = 0;
    const uint8_t *ptr = storage;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    memset(storage, 0, sizeof(storage));

    if (copy_len > 0)
        memcpy(storage, Data, copy_len);

    if (Size > 0)
        selector = Data[0];
    if (Size > 1)
        extra = (uint16_t)Data[1];
    if (Size > 3)
        extra = (uint16_t)((uint16_t)Data[1] | ((uint16_t)Data[2] << 8));
    if (Size > 4)
        remaining = (uint32_t)Data[3];
    if (Size > 7)
        remaining = (uint32_t)Data[3] |
                    ((uint32_t)Data[4] << 8) |
                    ((uint32_t)Data[5] << 16) |
                    ((uint32_t)Data[6] << 24);
    if (Size > 7)
        flags = Data[7];

    parser.source.end = storage + copy_len;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = ptr;
    value.remaining = remaining;
    value.extra = extra;
    value.flags = flags;

    {
        uint8_t interesting_types[] = {
            0,
            CborIntegerType,
            CborByteStringType,
            CborTextStringType,
            CborArrayType,
            CborMapType,
            CborTagType,
            CborSimpleType,
            CborBooleanType,
            CborNullType,
            CborUndefinedType,
            CborHalfFloatType,
            CborFloatType,
            CborDoubleType,
            0xff
        };
        size_t i;

        for (i = 0; i < sizeof(interesting_types) / sizeof(interesting_types[0]); i++) {
            value.type = interesting_types[i];
            exercise_value(&value);
        }
    }

    value.type = selector;
    exercise_value(&value);

    if (copy_len > 0) {
        size_t i;
        for (i = 0; i < copy_len; i++) {
            value.type = storage[i];
            value.extra = (uint16_t)((extra + storage[i]) & 0xffffu);
            value.remaining = remaining ^ (uint32_t)storage[i];
            value.flags = (uint8_t)(flags ^ storage[i]);
            value.source.ptr = storage + i;
            exercise_value(&value);
        }
    }

    return 0;
}