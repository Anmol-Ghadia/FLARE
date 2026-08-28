#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        int64_t s64 = 0;
        uint64_t u64 = 0;
        uint64_t raw = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int64_checked(it, &s64);

        if (cbor_value_is_unsigned_integer(it)) {
            (void)cbor_value_get_uint64(it, &u64);
        }
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 128) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;

    if (!Data)
        return 0;

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    walk_value(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;
        while (!cbor_value_at_end(&cur) && steps++ < 128) {
            exercise_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    return 0;
}