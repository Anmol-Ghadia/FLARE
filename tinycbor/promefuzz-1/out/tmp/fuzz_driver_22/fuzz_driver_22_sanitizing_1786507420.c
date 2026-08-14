#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_integer_apis(CborValue *it) {
    int i32 = 0;
    int i32_checked = 0;
    int64_t i64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_raw_integer(it, &raw);
    (void)cbor_value_get_int(it, &i32);
    (void)cbor_value_get_int64(it, &i64);
    (void)cbor_value_get_int_checked(it, &i32_checked);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);

    (void)raw;
    (void)i32;
    (void)i32_checked;
    (void)i64;
    (void)u64;
}

static void traverse_value(CborValue *it, int depth);

static void traverse_container(CborValue *it, int depth) {
    CborValue rec;
    CborError err;

    if (depth > 32)
        return;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec)) {
        traverse_value(&rec, depth + 1);
        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            return;
    }

    (void)cbor_value_leave_container(it, &rec);
}

static void traverse_value(CborValue *it, int depth) {
    exercise_integer_apis(it);

    if (cbor_value_is_container(it))
        traverse_container(it, depth);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    traverse_value(&it, 0);
    exercise_integer_apis(&it);

    return 0;
}