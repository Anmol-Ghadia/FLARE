#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include "cbor.h"

static void fuzz_value(const CborValue *it) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;
    int i = 0;

    if (!it)
        return;

    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_is_integer(it)) {
        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int_checked(it, &i);

        if (cbor_value_is_unsigned_integer(it))
            (void)cbor_value_get_uint64(it, &u64);

        if (raw <= (uint64_t)INT_MAX)
            (void)cbor_value_get_int_checked(it, &i);
    }
}

static void walk_value(CborValue *it, int depth) {
    CborValue rec;
    CborError err;
    int count = 0;

    if (!it || depth > 32)
        return;

    if (!cbor_value_is_valid(it))
        return;

    fuzz_value(it);

    if (!cbor_value_is_container(it))
        return;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError)
        return;

    while (cbor_value_is_valid(&rec) && !cbor_value_at_end(&rec) && count++ < 128) {
        walk_value(&rec, depth + 1);

        if (!cbor_value_is_valid(&rec) || cbor_value_at_end(&rec))
            break;

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;
    }

    if (cbor_value_is_valid(&rec) && cbor_value_at_end(&rec))
        (void)cbor_value_leave_container(it, &rec);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    CborValue cur;
    int steps = 0;

    if (!Data || Size == 0)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (!cbor_value_is_valid(&it))
        return 0;

    walk_value(&it, 0);

    cur = it;
    while (cbor_value_is_valid(&cur) && !cbor_value_at_end(&cur) && steps++ < 128) {
        fuzz_value(&cur);

        if (cbor_value_is_container(&cur)) {
            CborValue rec;
            int inner = 0;

            err = cbor_value_enter_container(&cur, &rec);
            if (err == CborNoError) {
                while (cbor_value_is_valid(&rec) && !cbor_value_at_end(&rec) && inner++ < 64) {
                    walk_value(&rec, 1);

                    if (!cbor_value_is_valid(&rec) || cbor_value_at_end(&rec))
                        break;

                    err = cbor_value_advance(&rec);
                    if (err != CborNoError)
                        break;
                }

                if (cbor_value_is_valid(&rec) && cbor_value_at_end(&rec))
                    (void)cbor_value_leave_container(&cur, &rec);
            }
        }

        if (!cbor_value_is_valid(&cur) || cbor_value_at_end(&cur))
            break;

        err = cbor_value_advance(&cur);
        if (err != CborNoError)
            break;
    }

    return 0;
}