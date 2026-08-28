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

static void exercise_value(const CborValue *value) {
    if (!value)
        return;

    (void)cbor_value_is_integer(value);
    (void)cbor_value_is_unsigned_integer(value);
    (void)cbor_value_is_negative_integer(value);

    if (cbor_value_is_integer(value)) {
        uint64_t raw = 0;
        int64_t i64 = 0;
        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64(value, &i64);

        if (cbor_value_is_unsigned_integer(value)) {
            uint64_t u64 = 0;
            (void)cbor_value_get_uint64(value, &u64);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    exercise_value(&it);

    if (cbor_value_is_container(&it)) {
        CborValue rec;
        if (cbor_value_enter_container(&it, &rec) == CborNoError) {
            size_t limit = 32;
            while (!cbor_value_at_end(&rec) && limit--) {
                exercise_value(&rec);

                if (cbor_value_is_container(&rec)) {
                    CborValue inner;
                    if (cbor_value_enter_container(&rec, &inner) == CborNoError) {
                        size_t inner_limit = 16;
                        while (!cbor_value_at_end(&inner) && inner_limit--) {
                            exercise_value(&inner);
                            if (cbor_value_advance(&inner) != CborNoError)
                                break;
                        }
                        (void)cbor_value_leave_container(&rec, &inner);
                    }
                }

                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(&it, &rec);
        }
    } else {
        CborValue tmp = it;
        (void)cbor_value_advance(&tmp);
    }

    if (Size > 0) {
        size_t off = Data[0] % Size;
        err = cbor_parser_init(Data + off, Size - off, 0, &parser, &it);
        if (err == CborNoError) {
            exercise_value(&it);
            if (!cbor_value_at_end(&it))
                (void)cbor_value_advance(&it);
        }
    }

    return 0;
}