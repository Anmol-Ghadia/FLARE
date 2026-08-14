#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    if (!it)
        return;

    bool is_int = cbor_value_is_integer(it);
    bool is_neg = cbor_value_is_negative_integer(it);
    bool is_unsigned = cbor_value_is_unsigned_integer(it);

    (void)is_neg;
    (void)is_unsigned;

    if (is_int) {
        uint64_t raw = 0;
        int64_t i64 = 0;
        int64_t i64_checked = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &i64);
        (void)cbor_value_get_int64_checked(it, &i64_checked);

        if (cbor_value_is_negative_integer(it)) {
            (void)raw;
            (void)i64;
            (void)i64_checked;
        }

        if (cbor_value_is_unsigned_integer(it)) {
            (void)raw;
            (void)i64;
            (void)i64_checked;
        }
    }
}

static void walk_container(CborValue *it, int depth) {
    if (!it || depth > 16)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&recursed) && count++ < 64) {
                walk_container(&recursed, depth + 1);
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        walk_container(&it, 0);

        CborValue cursor = it;
        int steps = 0;
        while (!cbor_value_at_end(&cursor) && steps++ < 64) {
            exercise_value(&cursor);
            err = cbor_value_advance(&cursor);
            if (err != CborNoError)
                break;
        }
    }

    if (Size > 0) {
        uint8_t buf[32];
        size_t n = Size < sizeof(buf) ? Size : sizeof(buf);
        memcpy(buf, Data, n);

        for (size_t i = 0; i < n; ++i) {
            buf[i] ^= (uint8_t)(1u << (i & 7));

            err = cbor_parser_init(buf, n, 0, &parser, &it);
            if (err == CborNoError) {
                walk_container(&it, 0);
                exercise_value(&it);
            }

            buf[i] ^= (uint8_t)(1u << (i & 7));
        }
    }

    return 0;
}