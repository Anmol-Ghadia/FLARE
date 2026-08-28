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

    if (Data == NULL)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    for (;;) {
        bool is_int;
        bool is_neg1, is_uns1, is_neg2, is_uns2;
        uint64_t raw = 0;
        int64_t val = 0;

        is_int = cbor_value_is_integer(&it);
        if (is_int) {
            (void)cbor_value_get_raw_integer(&it, &raw);
            is_neg1 = cbor_value_is_negative_integer(&it);
            is_uns1 = cbor_value_is_unsigned_integer(&it);
            is_neg2 = cbor_value_is_negative_integer(&it);
            is_uns2 = cbor_value_is_unsigned_integer(&it);
            (void)is_neg1;
            (void)is_uns1;
            (void)is_neg2;
            (void)is_uns2;
            (void)raw;
            (void)cbor_value_get_int64(&it, &val);
            (void)val;
        }

        if (cbor_value_at_end(&it))
            break;

        err = cbor_value_advance_fixed(&it);
        if (err != CborNoError) {
            err = cbor_value_advance(&it);
            if (err != CborNoError)
                break;
        }

        if (cbor_value_at_end(&it))
            break;
    }

    return 0;
}