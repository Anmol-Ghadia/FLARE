#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "cbor.h"

static void consume_integer_value(const CborValue *it) {
    int i = 0;
    int64_t i64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!cbor_value_is_valid(it))
        return;
    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_int_checked(it, &i);
    (void)cbor_value_get_int(it, &i);
    (void)cbor_value_get_int64(it, &i64);
    (void)cbor_value_get_raw_integer(it, &raw);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);
}

static void walk_value(CborValue *it, int depth) {
    if (depth > 32)
        return;
    if (!cbor_value_is_valid(it))
        return;

    consume_integer_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err;
        int count = 0;
        bool can_leave = false;

        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError || !cbor_value_is_valid(&rec))
            return;

        can_leave = true;

        while (cbor_value_is_valid(&rec) && !cbor_value_at_end(&rec) && count++ < 128) {
            walk_value(&rec, depth + 1);

            if (!cbor_value_is_valid(&rec) || cbor_value_at_end(&rec))
                break;

            err = cbor_value_advance(&rec);
            if (err != CborNoError) {
                can_leave = false;
                break;
            }
        }

        if (can_leave && cbor_value_is_valid(&rec))
            (void)cbor_value_leave_container(it, &rec);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL)
        return 0;

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0)
                (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError || !cbor_value_is_valid(&it))
        return 0;

    walk_value(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;

        while (cbor_value_is_valid(&cur) && !cbor_value_at_end(&cur) && steps++ < 128) {
            consume_integer_value(&cur);

            if (!cbor_value_is_valid(&cur) || cbor_value_at_end(&cur))
                break;

            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}