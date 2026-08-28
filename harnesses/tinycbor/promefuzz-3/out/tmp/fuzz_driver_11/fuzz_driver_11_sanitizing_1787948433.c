#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_value(const CborValue *value) {
    uint64_t u64 = 0, raw = 0;
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
    if (!it || depth > 16)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        if (cbor_value_enter_container(it, &recursed) == CborNoError) {
            int steps = 0;
            while (!cbor_value_at_end(&recursed) && steps++ < 32) {
                walk_value(&recursed, depth + 1);

                if (cbor_value_at_end(&recursed))
                    break;

                if (cbor_value_advance(&recursed) != CborNoError)
                    break;
            }

            if (cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

static void parse_and_walk(const uint8_t *data, size_t size) {
    CborParser parser;
    CborValue it;

    if (cbor_parser_init(data, size, 0, &parser, &it) != CborNoError)
        return;

    walk_value(&it, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    parse_and_walk(Data, Size);

    if (Size > 0) {
        size_t n = Size < 9 ? Size : 9;
        parse_and_walk(Data, n);
    }

    return 0;
}