#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
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

    CborValue cur = it;
    size_t steps = 0;

    while (!cbor_value_at_end(&cur) && steps++ < 1024) {
        if (cbor_value_is_integer(&cur)) {
            int64_t i64 = 0;
            int i32 = 0;
            uint64_t raw = 0;
            bool is_unsigned = cbor_value_is_unsigned_integer(&cur);

            (void)cbor_value_get_int64(&cur, &i64);
            (void)cbor_value_get_int(&cur, &i32);
            (void)cbor_value_get_int64_checked(&cur, &i64);
            (void)cbor_value_get_raw_integer(&cur, &raw);
            (void)cbor_value_get_int_checked(&cur, &i32);

            if (is_unsigned) {
                volatile uint64_t sink_u64 = raw + (uint64_t)i64 + (uint64_t)(unsigned int)i32;
                (void)sink_u64;
            } else {
                volatile int64_t sink_i64 = i64 + (int64_t)i32 - (int64_t)(raw & INT64_MAX);
                (void)sink_i64;
            }
        }

        if (cbor_value_is_container(&cur)) {
            CborValue recursed;
            if (cbor_value_enter_container(&cur, &recursed) == CborNoError) {
                CborValue inner = recursed;
                size_t inner_steps = 0;

                while (!cbor_value_at_end(&inner) && inner_steps++ < 64) {
                    if (cbor_value_is_integer(&inner)) {
                        int64_t i64 = 0;
                        int i32 = 0;
                        uint64_t raw = 0;

                        (void)cbor_value_is_unsigned_integer(&inner);
                        (void)cbor_value_get_int64(&inner, &i64);
                        (void)cbor_value_get_int(&inner, &i32);
                        (void)cbor_value_get_int64_checked(&inner, &i64);
                        (void)cbor_value_get_raw_integer(&inner, &raw);
                        (void)cbor_value_get_int_checked(&inner, &i32);

                        volatile uint64_t sink = raw ^ (uint64_t)i64 ^ (uint64_t)(unsigned int)i32;
                        (void)sink;
                    }

                    if (cbor_value_advance(&inner) != CborNoError)
                        break;
                }

                (void)cbor_value_leave_container(&cur, &inner);
            }
        }

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}