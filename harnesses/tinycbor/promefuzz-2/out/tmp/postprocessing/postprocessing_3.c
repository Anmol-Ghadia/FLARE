#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
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

    while (!cbor_value_at_end(&it)) {
        if (cbor_value_is_integer(&it)) {
            uint64_t raw = 0;
            int64_t val = 0;
            bool neg1, uns1, neg2, uns2;

            (void)cbor_value_get_raw_integer(&it, &raw);
            neg1 = cbor_value_is_negative_integer(&it);
            uns1 = cbor_value_is_unsigned_integer(&it);
            neg2 = cbor_value_is_negative_integer(&it);
            uns2 = cbor_value_is_unsigned_integer(&it);
            (void)neg1;
            (void)uns1;
            (void)neg2;
            (void)uns2;
            (void)raw;
            (void)cbor_value_get_int64(&it, &val);
            (void)val;
        }

        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}