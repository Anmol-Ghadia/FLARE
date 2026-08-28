#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

static int is_integer_type(uint8_t type) {
    return type == CborIntegerType || type == CborNegativeIntegerType;
}

static void exercise_value(const CborValue *value) {
    uint64_t raw = 0;
    int64_t i64 = 0;
    int i = 0;

    if (!is_integer_type(value->type))
        return;

    (void)_cbor_value_extract_int64_helper(value);
    (void)cbor_value_get_raw_integer(value, &raw);
    (void)cbor_value_get_int64(value, &i64);
    (void)cbor_value_get_int_checked(value, &i);
    (void)cbor_value_get_int(value, &i);

    if (value->flags & CborIteratorFlag_IntegerValueTooLarge)
        (void)_cbor_value_decode_int64_internal(value);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborValue v2;
    CborValue v3;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    memset(&v2, 0, sizeof(v2));
    memset(&v3, 0, sizeof(v3));

    value.parser = &parser;
    value.source.ptr = Data;
    value.type = CborIntegerType;

    if (Size > 0)
        value.type = (Data[0] & 1) ? CborNegativeIntegerType : CborIntegerType;
    if (Size > 1)
        value.flags = Data[1];
    if (Size > 3)
        value.extra = (uint16_t)Data[2] | ((uint16_t)Data[3] << 8);
    if (Size > 7)
        value.remaining = (uint32_t)Data[4] |
                          ((uint32_t)Data[5] << 8) |
                          ((uint32_t)Data[6] << 16) |
                          ((uint32_t)Data[7] << 24);

    exercise_value(&value);

    v2 = value;
    if (Size > 8)
        v2.flags ^= Data[8];
    if (Size > 10)
        v2.extra ^= (uint16_t)Data[9] | ((uint16_t)Data[10] << 8);
    if (Size > 0)
        v2.source.ptr = Data + (Size - 1);
    v2.type = (value.type == CborIntegerType) ? CborNegativeIntegerType : CborIntegerType;
    exercise_value(&v2);

    v3 = value;
    if (Size > 11)
        v3.type = (Data[11] & 1) ? CborNegativeIntegerType : CborIntegerType;
    if (Size > 12)
        v3.flags = Data[12];
    if (Size > 14)
        v3.extra = (uint16_t)Data[13] | ((uint16_t)Data[14] << 8);
    if (Size > 15)
        v3.remaining = Data[15];
    if (Size > 0)
        v3.source.ptr = Data + (Size / 2);
    exercise_value(&v3);

    return 0;
}