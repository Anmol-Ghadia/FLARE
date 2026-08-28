#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static void exercise_integer_accessors(CborValue *v) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!v)
        return;

    if (cbor_value_is_integer(v)) {
        (void)cbor_value_get_raw_integer(v, &raw);
        (void)cbor_value_get_int64(v, &s64);
        (void)cbor_value_get_int64_checked(v, &s64);

        if (cbor_value_is_unsigned_integer(v))
            (void)cbor_value_get_uint64(v, &u64);
    }
}

static void walk_value(CborValue *it, int depth) {
    int steps = 0;

    if (!it || depth > 3)
        return;

    while (!cbor_value_at_end(it) && steps < 128) {
        CborValue current = *it;

        exercise_integer_accessors(&current);

        if (cbor_value_is_container(&current)) {
            CborValue rec;
            if (cbor_value_enter_container(&current, &rec) == CborNoError) {
                walk_value(&rec, depth + 1);
                (void)cbor_value_leave_container(&current, &rec);
            }
        }

        if (cbor_value_advance(it) != CborNoError)
            break;
        ++steps;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    walk_value(&it, 0);
    return 0;
}