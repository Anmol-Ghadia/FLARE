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

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        CborValue cur = it;
        size_t steps = 0;

        while (steps++ < 16) {
            bool is_int = cbor_value_is_integer(&cur);

            if (is_int) {
                uint64_t raw = 0;
                int64_t i64 = 0;
                (void)cbor_value_get_raw_integer(&cur, &raw);
                (void)cbor_value_is_negative_integer(&cur);
                (void)cbor_value_is_unsigned_integer(&cur);
                (void)cbor_value_is_negative_integer(&cur);
                (void)cbor_value_is_unsigned_integer(&cur);
                (void)cbor_value_get_int64(&cur, &i64);

                if ((raw & 1u) != 0) {
                    (void)cbor_value_is_integer(&cur);
                    (void)cbor_value_get_raw_integer(&cur, &raw);
                    (void)cbor_value_is_negative_integer(&cur);
                    (void)cbor_value_is_unsigned_integer(&cur);
                    (void)cbor_value_is_negative_integer(&cur);
                    (void)cbor_value_is_unsigned_integer(&cur);
                    (void)cbor_value_get_int64(&cur, &i64);
                }
            }

            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}