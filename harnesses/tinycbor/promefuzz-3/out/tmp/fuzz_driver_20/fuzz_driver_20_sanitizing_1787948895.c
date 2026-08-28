#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
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

static uint64_t read_u64(const uint8_t *p) {
    return (uint64_t)p[0]
         | ((uint64_t)p[1] << 8)
         | ((uint64_t)p[2] << 16)
         | ((uint64_t)p[3] << 24)
         | ((uint64_t)p[4] << 32)
         | ((uint64_t)p[5] << 40)
         | ((uint64_t)p[6] << 48)
         | ((uint64_t)p[7] << 56);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t localbuf[16] = {0};
    CborParser parser;
    CborValue value;
    uint64_t raw = 0, helper = 0, decoded = 0;
    int64_t i64 = 0;
    int i32 = 0;
    CborError err;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    if (Size > 0) {
        size_t copy = Size < sizeof(localbuf) ? Size : sizeof(localbuf);
        memcpy(localbuf, Data, copy);
    }

    parser.source.end = localbuf + sizeof(localbuf);
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = localbuf;
    value.remaining = (Size >= 4) ? read_u32(Data) : 0;
    value.extra = (Size >= 6) ? read_u16(Data + 4) : 0;

    if (Size > 6) {
        value.flags = Data[6];
    } else {
        value.flags = 0;
    }

    if (Size > 7) {
        value.type = (Data[7] & 1) ? CborNegativeIntegerType : CborIntegerType;
    } else {
        value.type = CborIntegerType;
    }

    value.flags &= (uint8_t)(CborIteratorFlag_IntegerValueTooLarge |
                             CborIteratorFlag_NegativeInteger |
                             CborIteratorFlag_IntegerValueIs64Bit);

    if (value.type == CborNegativeIntegerType)
        value.flags |= CborIteratorFlag_NegativeInteger;
    else
        value.flags &= (uint8_t)~CborIteratorFlag_NegativeInteger;

    if (Size >= 16) {
        memcpy(localbuf, Data + Size - 8, 8);
    } else if (Size >= 8) {
        size_t n = Size - 8;
        if (n > sizeof(localbuf))
            n = sizeof(localbuf);
        memcpy(localbuf, Data + 8, n);
    }

    helper = _cbor_value_extract_int64_helper(&value);
    (void)helper;

    err = cbor_value_get_raw_integer(&value, &raw);
    (void)err;
    (void)raw;

    err = cbor_value_get_int64(&value, &i64);
    (void)err;
    (void)i64;

    err = cbor_value_get_int_checked(&value, &i32);
    (void)err;
    (void)i32;

    err = cbor_value_get_int(&value, &i32);
    (void)err;
    (void)i32;

    if (value.flags & CborIteratorFlag_IntegerValueTooLarge) {
        decoded = _cbor_value_decode_int64_internal(&value);
        (void)decoded;
    }

    value.flags ^= CborIteratorFlag_IntegerValueTooLarge;
    helper = _cbor_value_extract_int64_helper(&value);
    (void)helper;

    err = cbor_value_get_raw_integer(&value, &raw);
    (void)err;

    err = cbor_value_get_int64(&value, &i64);
    (void)err;

    err = cbor_value_get_int_checked(&value, &i32);
    (void)err;

    err = cbor_value_get_int(&value, &i32);
    (void)err;

    value.flags ^= CborIteratorFlag_IntegerValueIs64Bit;
    if (value.flags & CborIteratorFlag_IntegerValueTooLarge) {
        decoded = _cbor_value_decode_int64_internal(&value);
        (void)decoded;
    }

    if (Size >= 8) {
        value.extra = (uint16_t)(read_u64(Data) & 0xffffu);
        value.remaining ^= read_u32(Data + (Size >= 12 ? 8 : 4));
    } else if (Size >= 4) {
        value.extra ^= read_u16(Data + 2);
        value.remaining ^= read_u32(Data);
    }

    helper = _cbor_value_extract_int64_helper(&value);
    (void)helper;

    err = cbor_value_get_raw_integer(&value, &raw);
    (void)err;

    err = cbor_value_get_int64(&value, &i64);
    (void)err;

    err = cbor_value_get_int_checked(&value, &i32);
    (void)err;

    err = cbor_value_get_int(&value, &i32);
    (void)err;

    return 0;
}