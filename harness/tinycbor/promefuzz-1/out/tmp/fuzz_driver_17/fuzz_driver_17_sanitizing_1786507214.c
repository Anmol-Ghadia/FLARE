#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        uint64_t raw = 0;
        int64_t i64 = 0;
        int64_t i64_checked = 0;

        (void)cbor_value_is_negative_integer(it);
        (void)cbor_value_is_unsigned_integer(it);
        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &i64);
        (void)cbor_value_get_int64_checked(it, &i64_checked);

        (void)raw;
        (void)i64;
        (void)i64_checked;
    } else {
        (void)cbor_value_is_negative_integer(it);
        (void)cbor_value_is_unsigned_integer(it);
    }
}

static void walk_value(CborValue *it, int depth);

static void walk_container(CborValue *container, int depth) {
    CborValue recursed;
    CborError err;
    int count = 0;

    if (!container || depth > 16)
        return;

    err = cbor_value_enter_container(container, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed) && count++ < 64) {
        walk_value(&recursed, depth + 1);

        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            return;
    }

    if (cbor_value_at_end(&recursed))
        (void)cbor_value_leave_container(container, &recursed);
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 16)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it))
        walk_container(it, depth);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError)
        walk_value(&it, 0);

    if (Size > 0) {
        uint8_t buf[32];
        size_t n = Size < sizeof(buf) ? Size : sizeof(buf);

        memcpy(buf, Data, n);
        for (size_t i = 0; i < n; ++i) {
            buf[i] ^= (uint8_t)(1u << (i & 7));

            err = cbor_parser_init(buf, n, 0, &parser, &it);
            if (err == CborNoError)
                walk_value(&it, 0);

            buf[i] ^= (uint8_t)(1u << (i & 7));
        }
    }

    return 0;
}