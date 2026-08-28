#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!it)
        return;

    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_raw_integer(it, &raw);
    (void)cbor_value_get_int64(it, &s64);
    (void)cbor_value_get_int64_checked(it, &s64);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err;
        int count = 0;

        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&rec) && count++ < 128) {
            walk_value(&rec, depth + 1);
            err = cbor_value_advance(&rec);
            if (err != CborNoError)
                break;
        }

        /* cbor_value_leave_container asserts that rec is at end/invalid.
           Only call it when the recursive iterator reached the end cleanly. */
        if (cbor_value_at_end(&rec))
            (void)cbor_value_leave_container(it, &rec);
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

    walk_value(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;

        while (!cbor_value_at_end(&cur) && steps++ < 128) {
            exercise_value(&cur);
            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}