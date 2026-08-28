#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

#ifndef CborIntegerType
#define CborIntegerType 0x00
#endif

#ifndef CborNegativeIntegerType
#define CborNegativeIntegerType 0x20
#endif

#ifndef CborIteratorFlag_IntegerValueTooLarge
#define CborIteratorFlag_IntegerValueTooLarge 0x01
#endif

#ifndef CborIteratorFlag_NegativeInteger
#define CborIteratorFlag_NegativeInteger 0x02
#endif

#ifndef CborIteratorFlag_IntegerValueIs64Bit
#define CborIteratorFlag_IntegerValueIs64Bit 0x04
#endif

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static void init_value(CborValue *value, CborParser *parser, const uint8_t *buf, size_t Size) {
    memset(parser, 0, sizeof(*parser));
    memset(value, 0, sizeof(*value));

    parser->source.end = buf + 16;
    parser->flags = 0;

    value->parser = parser;
    value->source.ptr = buf;
    value->remaining = (Size >= 4) ? read_u32(buf) : 0;
    value->extra = (Size >= 6) ? read_u16(buf + 4) : 0;
    value->flags = (Size > 6) ? buf[6] : 0;
    value->type = (Size > 7 && (buf[7] & 1)) ? CborNegativeIntegerType : CborIntegerType;

    value->flags &= (uint8_t)(CborIteratorFlag_IntegerValueTooLarge |
                              CborIteratorFlag_NegativeInteger |
                              CborIteratorFlag_IntegerValueIs64Bit);

    if (value->type == CborNegativeIntegerType)
        value->flags |= CborIteratorFlag_NegativeInteger;
    else
        value->flags &= (uint8_t)~CborIteratorFlag_NegativeInteger;
}

static void exercise_integer_apis(CborValue *value) {
    uint64_t raw = 0;
    uint64_t helper = 0;
    int64_t i64 = 0;
    int i32 = 0;
    CborError err;

    helper = _cbor_value_extract_int64_helper(value);
    (void)helper;

    err = cbor_value_get_raw_integer(value, &raw);
    (void)err;
    (void)raw;

    err = cbor_value_get_int64(value, &i64);
    (void)err;
    (void)i64;

    err = cbor_value_get_int_checked(value, &i32);
    (void)err;
    (void)i32;

    err = cbor_value_get_int(value, &i32);
    (void)err;
    (void)i32;

    if (value->flags & CborIteratorFlag_IntegerValueTooLarge) {
        uint64_t decoded = _cbor_value_decode_int64_internal(value);
        (void)decoded;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t localbuf[16] = {0};
    CborParser parser;
    CborValue value;

    if (Size > 0) {
        size_t copy = Size < sizeof(localbuf) ? Size : sizeof(localbuf);
        memcpy(localbuf, Data, copy);
    }

    init_value(&value, &parser, localbuf, Size);
    exercise_integer_apis(&value);

    init_value(&value, &parser, localbuf, Size);
    value.flags ^= CborIteratorFlag_IntegerValueTooLarge;
    exercise_integer_apis(&value);

    init_value(&value, &parser, localbuf, Size);
    value.flags |= CborIteratorFlag_IntegerValueTooLarge;
    value.flags ^= CborIteratorFlag_IntegerValueIs64Bit;
    exercise_integer_apis(&value);

    init_value(&value, &parser, localbuf, Size);
    value.type = (value.type == CborIntegerType) ? CborNegativeIntegerType : CborIntegerType;
    if (value.type == CborNegativeIntegerType)
        value.flags |= CborIteratorFlag_NegativeInteger;
    else
        value.flags &= (uint8_t)~CborIteratorFlag_NegativeInteger;
    exercise_integer_apis(&value);

    init_value(&value, &parser, localbuf, Size);
    if (Size >= 8) {
        value.extra = (uint16_t)(read_u32(localbuf + 2) & 0xffffu);
        value.remaining ^= read_u32(localbuf + 4);
    } else if (Size >= 4) {
        value.extra ^= read_u16(localbuf + 2);
        value.remaining ^= read_u32(localbuf);
    }
    exercise_integer_apis(&value);

    return 0;
}