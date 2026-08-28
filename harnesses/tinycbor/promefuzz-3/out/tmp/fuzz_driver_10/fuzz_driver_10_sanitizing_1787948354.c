#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
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

static void walk_value(CborValue *it, int depth) {
    size_t limit = 32;

    if (!it || depth > 3)
        return;

    while (!cbor_value_at_end(it) && limit--) {
        exercise_value(it);

        if (cbor_value_is_container(it)) {
            CborValue rec;
            if (cbor_value_enter_container(it, &rec) != CborNoError)
                return;

            walk_value(&rec, depth + 1);

            while (!cbor_value_at_end(&rec)) {
                if (cbor_value_is_container(&rec)) {
                    CborValue nested;
                    if (cbor_value_enter_container(&rec, &nested) != CborNoError)
                        break;
                    walk_value(&nested, depth + 1);
                    while (!cbor_value_at_end(&nested)) {
                        if (cbor_value_advance(&nested) != CborNoError)
                            break;
                    }
                    if (cbor_value_at_end(&nested))
                        (void)cbor_value_leave_container(&rec, &nested);
                    else
                        break;
                } else {
                    if (cbor_value_advance(&rec) != CborNoError)
                        break;
                }
            }

            if (cbor_value_at_end(&rec))
                (void)cbor_value_leave_container(it, &rec);
            else
                return;

            continue;
        }

        if (cbor_value_advance(it) != CborNoError)
            return;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError)
        walk_value(&it, 0);

    if (Size > 0) {
        size_t off = Data[0] % Size;
        err = cbor_parser_init(Data + off, Size - off, 0, &parser, &it);
        if (err == CborNoError)
            walk_value(&it, 0);
    }

    return 0;
}