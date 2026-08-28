// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_is_boolean at cbor.h:391:22 in cbor.h
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_is_undefined at cbor.h:387:22 in cbor.h
// cbor_value_is_null at cbor.h:385:22 in cbor.h
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

static void exercise_checks(const CborValue *value) {
    (void)cbor_value_is_simple_type(value);
    (void)cbor_value_is_boolean(value);
    (void)cbor_value_is_half_float(value);
    (void)cbor_value_is_double(value);
    (void)cbor_value_is_undefined(value);
    (void)cbor_value_is_null(value);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)Size;
    value.extra = (Size >= 2) ? (uint16_t)((Data[0] << 8) | Data[1]) : (uint16_t)Size;
    value.flags = (Size >= 3) ? Data[2] : 0;

    exercise_checks(&value);

    if (Size > 0) {
        value.type = Data[0];
        exercise_checks(&value);
    }

    value.type = CborSimpleType;
    exercise_checks(&value);

    value.type = CborBooleanType;
    exercise_checks(&value);

    value.type = CborHalfFloatType;
    exercise_checks(&value);

    value.type = CborDoubleType;
    exercise_checks(&value);

    value.type = CborUndefinedType;
    exercise_checks(&value);

    value.type = CborNullType;
    exercise_checks(&value);

    if (Size > 1) {
        for (size_t i = 0; i < Size; ++i) {
            value.type = Data[i];
            value.remaining = (uint32_t)(Size - i);
            value.flags ^= Data[i];
            exercise_checks(&value);
        }
    }

    return 0;
}