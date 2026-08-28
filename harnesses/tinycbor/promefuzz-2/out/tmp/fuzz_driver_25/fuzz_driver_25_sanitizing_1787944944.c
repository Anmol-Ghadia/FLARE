#include <stdint.h>
#include <stddef.h>
#include "cbor.h"

static void exercise_integer_accessors(const CborValue *it) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!it || !cbor_value_is_valid(it))
        return;

    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_raw_integer(it, &raw);
    (void)cbor_value_get_int64(it, &s64);
    (void)cbor_value_get_int64_checked(it, &s64);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);
}

static void fuzz_value(const CborValue *it, size_t depth) {
    CborValue rec;
    CborValue cur;
    CborError err;

    if (!it || depth > 32 || !cbor_value_is_valid(it))
        return;

    exercise_integer_accessors(it);

    if (!cbor_value_is_container(it))
        return;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError || !cbor_value_is_valid(&rec))
        return;

    cur = rec;
    while (cbor_value_is_valid(&cur) && !cbor_value_at_end(&cur)) {
        exercise_integer_accessors(&cur);

        if (cbor_value_is_container(&cur))
            fuzz_value(&cur, depth + 1);

        err = cbor_value_advance(&cur);
        if (err != CborNoError)
            break;
    }

    if (err == CborNoError && cbor_value_is_valid(&cur) && cbor_value_at_end(&cur))
        (void)cbor_value_leave_container(it, &cur);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError || !cbor_value_is_valid(&it))
        return 0;

    exercise_integer_accessors(&it);
    fuzz_value(&it, 0);

    return 0;
}