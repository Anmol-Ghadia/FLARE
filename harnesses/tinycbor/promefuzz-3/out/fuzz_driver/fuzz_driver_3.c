// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
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

    if (it.type == CborInvalidType)
        return 0;

    {
        bool is_int = cbor_value_is_integer(&it);

        if (is_int) {
            uint64_t raw = 0;
            int64_t i64 = 0;

            (void)cbor_value_get_raw_integer(&it, &raw);
            (void)cbor_value_is_negative_integer(&it);
            (void)cbor_value_is_unsigned_integer(&it);
            (void)cbor_value_is_negative_integer(&it);
            (void)cbor_value_is_unsigned_integer(&it);
            (void)cbor_value_get_int64(&it, &i64);

            if ((raw & 1u) != 0) {
                (void)cbor_value_is_integer(&it);
                (void)cbor_value_get_raw_integer(&it, &raw);
                (void)cbor_value_is_negative_integer(&it);
                (void)cbor_value_is_unsigned_integer(&it);
                (void)cbor_value_is_negative_integer(&it);
                (void)cbor_value_is_unsigned_integer(&it);
                (void)cbor_value_get_int64(&it, &i64);
            }
        }
    }

    return 0;
}