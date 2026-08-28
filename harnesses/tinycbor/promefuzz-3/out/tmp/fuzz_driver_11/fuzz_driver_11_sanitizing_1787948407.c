#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
    if (!it || depth > 16 || cbor_value_at_end(it))
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        if (cbor_value_enter_container(it, &recursed) == CborNoError) {
            int steps = 0;
            while (!cbor_value_at_end(&recursed) && steps++ < 32) {
                walk_value(&recursed, depth + 1);
                if (cbor_value_advance(&recursed) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    CborValue cur = it;
    int top_steps = 0;
    while (!cbor_value_at_end(&cur) && top_steps++ < 32) {
        walk_value(&cur, 0);
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    if (Size > 0) {
        uint8_t smallbuf[9];
        size_t n = Size < sizeof(smallbuf) ? Size : sizeof(smallbuf);
        memcpy(smallbuf, Data, n);

        if (cbor_parser_init(smallbuf, n, 0, &parser, &it) == CborNoError) {
            CborValue tmp = it;
            int steps = 0;
            while (!cbor_value_at_end(&tmp) && steps++ < 8) {
                walk_value(&tmp, 0);
                if (cbor_value_advance(&tmp) != CborNoError)
                    break;
            }
        }
    }

    return 0;
}