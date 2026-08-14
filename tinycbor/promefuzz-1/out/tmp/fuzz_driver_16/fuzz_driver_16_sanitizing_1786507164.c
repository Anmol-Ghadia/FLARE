#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cbor.h>

static void exercise_value(const CborValue *value) {
    uint64_t raw = 0;
    int64_t i64 = 0;
    int64_t checked = 0;

    if (!value)
        return;

    (void)cbor_value_is_integer(value);
    (void)cbor_value_is_negative_integer(value);
    (void)cbor_value_is_unsigned_integer(value);

    if (!cbor_value_is_integer(value))
        return;

    (void)cbor_value_get_raw_integer(value, &raw);
    (void)cbor_value_get_int64(value, &i64);
    (void)cbor_value_get_int64_checked(value, &checked);

    if (cbor_value_is_negative_integer(value)) {
        (void)cbor_value_get_int64(value, &i64);
        (void)cbor_value_get_int64_checked(value, &checked);
    }

    if (cbor_value_is_unsigned_integer(value)) {
        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64_checked(value, &checked);
    }
}

static void walk_top_level(CborValue *it, int max_items) {
    int count = 0;

    if (!it)
        return;

    while (!cbor_value_at_end(it) && count++ < max_items) {
        CborType type = cbor_value_get_type(it);
        CborError err;

        if (type == CborInvalidType)
            break;

        exercise_value(it);

        err = cbor_value_advance_fixed(it);
        if (err != CborNoError)
            break;
    }
}

static void fuzz_buffer(const uint8_t *buf, size_t size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(buf, size, 0, &parser, &it);
    if (err != CborNoError)
        return;

    if (cbor_value_get_type(&it) == CborInvalidType)
        return;

    exercise_value(&it);

    {
        CborValue tmp = it;
        walk_top_level(&tmp, 128);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    fuzz_buffer(Data, Size);

    if (Size > 0) {
        uint8_t *copy = (uint8_t *)malloc(Size);
        if (copy) {
            memcpy(copy, Data, Size);
            fuzz_buffer(copy, Size);
            free(copy);
        }
    }

    if (Size >= 1) {
        uint8_t *mut = (uint8_t *)malloc(Size);
        if (mut) {
            memcpy(mut, Data, Size);

            mut[0] ^= 0x20;
            fuzz_buffer(mut, Size);

            mut[0] ^= 0x20;
            mut[0] ^= 0x40;
            fuzz_buffer(mut, Size);

            free(mut);
        }
    }

    return 0;
}