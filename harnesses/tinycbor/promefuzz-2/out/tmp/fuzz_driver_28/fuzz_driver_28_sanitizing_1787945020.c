#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;
    int i = 0;

    if (!it)
        return;

    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_int64(it, &s64);
    (void)cbor_value_get_raw_integer(it, &raw);
    (void)cbor_value_get_int_checked(it, &i);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);

    if (cbor_value_is_negative_integer(it)) {
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int_checked(it, &i);
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err;
        int count = 0;

        memset(&rec, 0, sizeof(rec));
        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&rec) && count++ < 64) {
            walk_value(&rec, depth + 1);
            if (cbor_value_advance(&rec) != CborNoError)
                break;
        }

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

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    walk_value(&it, 0);

    return 0;
}