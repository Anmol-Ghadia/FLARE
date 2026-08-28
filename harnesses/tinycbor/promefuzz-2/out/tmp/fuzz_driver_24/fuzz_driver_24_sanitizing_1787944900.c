#include <stdint.h>
#include <stddef.h>
#include "cbor.h"

static void process_value(CborValue *it, int depth);

static void process_container(CborValue *it, int depth) {
    CborValue rec;
    CborError err;
    size_t limit = 32;

    if (depth > 8)
        return;
    if (!cbor_value_is_container(it))
        return;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec) && limit--) {
        process_value(&rec, depth + 1);

        if (cbor_value_at_end(&rec))
            break;

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;
    }

    if (cbor_value_at_end(&rec))
        (void)cbor_value_leave_container(it, &rec);
}

static void process_value(CborValue *it, int depth) {
    if (cbor_value_at_end(it))
        return;

    if (cbor_value_is_integer(it)) {
        int64_t s64 = 0;
        uint64_t u64 = 0;
        uint64_t raw = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int64_checked(it, &s64);

        if (cbor_value_is_unsigned_integer(it))
            (void)cbor_value_get_uint64(it, &u64);
    }

    if (cbor_value_is_container(it))
        process_container(it, depth);
}

static void fuzz_one_item(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    if (cbor_value_at_end(&it))
        return;

    process_value(&it, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    fuzz_one_item(Data, Size);

    if (Size > 1) {
        size_t split = (size_t)Data[0] % Size;
        fuzz_one_item(Data, split);
        fuzz_one_item(Data + split, Size - split);
    }

    return 0;
}