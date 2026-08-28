// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// _cbor_value_decode_int64_internal at cborparser.c:316:10 in cbor.h
// _cbor_value_extract_int64_helper at cbor.h:373:26 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int_checked at cborparser.c:843:11 in cbor.h
// cbor_value_get_int at cbor.h:441:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

static int has_bytes(const uint8_t *base, size_t size, const uint8_t *ptr, size_t need) {
    size_t off;
    if (ptr < base)
        return 0;
    off = (size_t)(ptr - base);
    return off <= size && need <= size - off;
}

static void exercise_value(const uint8_t *base, size_t size, const CborValue *value) {
    uint64_t raw = 0;
    int64_t i64 = 0;
    int i = 0;

    if ((value->flags & CborIteratorFlag_IntegerValueTooLarge) != 0) {
        size_t need = (value->flags & CborIteratorFlag_IntegerValueIs64Bit) ? 9u : 5u;
        if (!has_bytes(base, size, value->source.ptr, need))
            return;
        (void)_cbor_value_decode_int64_internal(value);
    }

    (void)_cbor_value_extract_int64_helper(value);
    (void)cbor_value_get_raw_integer(value, &raw);
    (void)cbor_value_get_int64(value, &i64);
    (void)cbor_value_get_int_checked(value, &i);
    (void)cbor_value_get_int(value, &i);
}

int LLVMFuzzerTestOneInput_21(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborValue v2;
    CborValue v3;
    size_t off = 0;

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
    if (Size > 0) {
        off = (Size > 7) ? ((size_t)Data[7] % Size) : 0;
        value.source.ptr = Data + off;
    }

    exercise_value(Data, Size, &value);

    v2 = value;
    if (Size > 8)
        v2.flags ^= Data[8];
    if (Size > 10)
        v2.extra ^= (uint16_t)Data[9] | ((uint16_t)Data[10] << 8);
    if (Size > 11)
        v2.remaining ^= (uint32_t)Data[11];
    if (Size > 0 && Size > 12) {
        off = (size_t)Data[12] % Size;
        v2.source.ptr = Data + off;
    }
    exercise_value(Data, Size, &v2);

    v3 = value;
    if (Size > 13)
        v3.flags = Data[13];
    if (Size > 15)
        v3.extra = (uint16_t)Data[14] | ((uint16_t)Data[15] << 8);
    if (Size > 16)
        v3.remaining = Data[16];
    if (Size > 0 && Size > 17) {
        off = (size_t)Data[17] % Size;
        v3.source.ptr = Data + off;
    }
    exercise_value(Data, Size, &v3);

    return 0;
}