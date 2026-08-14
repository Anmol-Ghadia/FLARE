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
    CborValue it;
    CborError err;

    if (Data == NULL || Size == 0)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        bool is_int = cbor_value_is_integer(&it);
        uint64_t raw = 0;
        int64_t val64 = 0;
        bool is_neg1 = false;
        bool is_unsigned1 = false;
        bool is_neg2 = false;
        bool is_unsigned2 = false;

        if (is_int) {
            (void)cbor_value_get_raw_integer(&it, &raw);
            is_neg1 = cbor_value_is_negative_integer(&it);
            is_unsigned1 = cbor_value_is_unsigned_integer(&it);
            is_neg2 = cbor_value_is_negative_integer(&it);
            is_unsigned2 = cbor_value_is_unsigned_integer(&it);
            (void)cbor_value_get_int64(&it, &val64);

            if (is_neg1 != is_neg2 || is_unsigned1 != is_unsigned2) {
                volatile uint64_t sink = raw ^ (uint64_t)val64;
                (void)sink;
            }
        }
    }

    if (Size > 1) {
        CborValue manual;
        memset(&manual, 0, sizeof(manual));
        manual.parser = &parser;
        manual.source.ptr = Data;
        manual.remaining = (uint32_t)(Size & 0xffffffffu);
        manual.extra = (uint16_t)((Size > 2) ? ((Data[1] << 8) | Data[Size - 1]) : Data[0]);
        manual.type = Data[0];
        manual.flags = (Size > 3) ? Data[2] : 0;

        if (cbor_value_is_integer(&manual)) {
            uint64_t raw = 0;
            int64_t val64 = 0;
            (void)cbor_value_get_raw_integer(&manual, &raw);
            (void)cbor_value_is_negative_integer(&manual);
            (void)cbor_value_is_unsigned_integer(&manual);
            (void)cbor_value_is_negative_integer(&manual);
            (void)cbor_value_is_unsigned_integer(&manual);
            (void)cbor_value_get_int64(&manual, &val64);
        }
    }

    return 0;
}