#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

static void exercise_value(const CborValue *value) {
    uint64_t raw = 0;
    int64_t i64 = 0;
    int i = 0;

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

    parser.source.end = Data + Size;

    value.parser = &parser;
    value.source.ptr = Data;
    value.type = CborIntegerType;
    value.flags = 0;
    value.extra = 0;
    value.remaining = 0;

    if (Size > 0)
        value.flags = Data[0];
    if (Size > 2)
        value.extra = (uint16_t)Data[1] | ((uint16_t)Data[2] << 8);
    if (Size > 6)
        value.remaining = (uint32_t)Data[3] |
                          ((uint32_t)Data[4] << 8) |
                          ((uint32_t)Data[5] << 16) |
                          ((uint32_t)Data[6] << 24);
    if (Size > 7)
        value.source.ptr = Data + (Data[7] % Size);

    exercise_value(&value);

    v2 = value;
    if (Size > 8)
        v2.flags ^= Data[8];
    if (Size > 10)
        v2.extra ^= (uint16_t)Data[9] | ((uint16_t)Data[10] << 8);
    if (Size > 11)
        v2.remaining ^= (uint32_t)Data[11];
    if (Size > 12)
        v2.source.ptr = Data + (Data[12] % Size);
    exercise_value(&v2);

    v3 = value;
    if (Size > 13)
        v3.flags = Data[13];
    if (Size > 15)
        v3.extra = (uint16_t)Data[14] | ((uint16_t)Data[15] << 8);
    if (Size > 16)
        v3.remaining = Data[16];
    if (Size > 17)
        v3.source.ptr = Data + (Data[17] % Size);
    exercise_value(&v3);

    return 0;
}