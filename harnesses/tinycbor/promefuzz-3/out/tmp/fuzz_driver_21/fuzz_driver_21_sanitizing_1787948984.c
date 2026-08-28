#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    memset(&parser, 0, sizeof(parser));

    CborValue value;
    memset(&value, 0, sizeof(value));
    value.parser = &parser;

    if (Size > 0)
        value.type = Data[0];
    if (Size > 1)
        value.flags = Data[1];
    if (Size > 3)
        value.extra = (uint16_t)((uint16_t)Data[2] | ((uint16_t)Data[3] << 8));
    if (Size > 7)
        value.remaining = (uint32_t)Data[4] |
                          ((uint32_t)Data[5] << 8) |
                          ((uint32_t)Data[6] << 16) |
                          ((uint32_t)Data[7] << 24);

    value.source.ptr = Data;

    {
        uint64_t raw = 0;
        int64_t i64 = 0;
        int i = 0;
        uint64_t helper = 0;
        uint64_t decoded = 0;

        helper = _cbor_value_extract_int64_helper(&value);
        (void)helper;

        cbor_value_get_raw_integer(&value, &raw);
        cbor_value_get_int64(&value, &i64);
        cbor_value_get_int_checked(&value, &i);
        decoded = _cbor_value_decode_int64_internal(&value);
        (void)decoded;
        cbor_value_get_int(&value, &i);
    }

    if (Size >= 8) {
        CborValue v2 = value;
        static const uint8_t int_types[] = { 0x00, 0x01 };
        size_t idx;

        for (idx = 0; idx < sizeof(int_types); ++idx) {
            uint64_t raw = 0;
            int64_t i64 = 0;
            int i = 0;

            v2.type = int_types[idx];
            v2.flags ^= (uint8_t)(Size > 8 ? Data[8] : 0);
            v2.extra ^= (uint16_t)(Size > 10 ? ((uint16_t)Data[9] | ((uint16_t)Data[10] << 8)) : 0);
            v2.source.ptr = Data + (Size > 0 ? (Size - 1) : 0);

            (void)_cbor_value_extract_int64_helper(&v2);
            cbor_value_get_raw_integer(&v2, &raw);
            cbor_value_get_int64(&v2, &i64);
            cbor_value_get_int_checked(&v2, &i);
            (void)_cbor_value_decode_int64_internal(&v2);
            cbor_value_get_int(&v2, &i);
        }
    }

    if (Size >= 16) {
        CborValue v3;
        memset(&v3, 0, sizeof(v3));
        v3.parser = &parser;
        v3.type = Data[11];
        v3.flags = Data[12];
        v3.extra = (uint16_t)((uint16_t)Data[13] | ((uint16_t)Data[14] << 8));
        v3.remaining = Data[15];
        v3.source.ptr = Data + (Size / 2);

        {
            uint64_t raw = 0;
            int64_t i64 = 0;
            int i = 0;

            (void)_cbor_value_extract_int64_helper(&v3);
            cbor_value_get_raw_integer(&v3, &raw);
            cbor_value_get_int64(&v3, &i64);
            cbor_value_get_int_checked(&v3, &i);
            (void)_cbor_value_decode_int64_internal(&v3);
            cbor_value_get_int(&v3, &i);
        }
    }

    return 0;
}