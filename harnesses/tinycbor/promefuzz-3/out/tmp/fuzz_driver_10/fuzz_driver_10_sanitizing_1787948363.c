#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_value(const CborValue *value) {
    uint64_t raw = 0, u64 = 0;
    int64_t i64 = 0;

    if (!value)
        return;

    (void)cbor_value_is_integer(value);
    (void)cbor_value_is_unsigned_integer(value);
    (void)cbor_value_is_negative_integer(value);

    if (cbor_value_is_integer(value)) {
        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64(value, &i64);

        if (cbor_value_is_unsigned_integer(value))
            (void)cbor_value_get_uint64(value, &u64);
    }
}

static void walk_value(CborValue *it, int depth) {
    size_t limit = 64;

    if (!it || depth > 4)
        return;

    while (!cbor_value_at_end(it) && limit--) {
        exercise_value(it);

        if (cbor_value_is_container(it)) {
            CborValue rec;
            CborError err = cbor_value_enter_container(it, &rec);
            if (err != CborNoError)
                return;

            walk_value(&rec, depth + 1);

            if (cbor_value_leave_container(it, &rec) != CborNoError)
                return;

            continue;
        }

        if (cbor_value_advance_fixed(it) != CborNoError)
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

    if (Size > 1) {
        size_t off = Data[0] % Size;
        err = cbor_parser_init(Data + off, Size - off, 0, &parser, &it);
        if (err == CborNoError)
            walk_value(&it, 0);
    }

    return 0;
}