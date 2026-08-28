// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int at cbor.h:441:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int_checked at cborparser.c:843:11 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int at cbor.h:441:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int_checked at cborparser.c:843:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_17(const uint8_t *Data, size_t Size) {
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
            }
        }

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}