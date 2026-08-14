#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
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

    if (cbor_value_is_integer(&it)) {
        uint64_t raw = 0;
        int64_t val64 = 0;
        bool is_neg1, is_unsigned1, is_neg2, is_unsigned2;

        (void)cbor_value_get_raw_integer(&it, &raw);
        is_neg1 = cbor_value_is_negative_integer(&it);
        is_unsigned1 = cbor_value_is_unsigned_integer(&it);
        is_neg2 = cbor_value_is_negative_integer(&it);
        is_unsigned2 = cbor_value_is_unsigned_integer(&it);
        (void)cbor_value_get_int64(&it, &val64);

        if ((is_neg1 != is_neg2) || (is_unsigned1 != is_unsigned2)) {
            volatile uint64_t sink = raw ^ (uint64_t)val64;
            (void)sink;
        }
    }

    return 0;
}