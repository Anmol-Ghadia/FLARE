#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static void consume_value(CborValue *it, int depth);

static void consume_container(CborValue *it, int depth) {
    CborValue recursed;
    CborError err;

    if (depth > 16)
        return;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        consume_value(&recursed, depth + 1);

        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void consume_value(CborValue *it, int depth) {
    CborType type = cbor_value_get_type(it);

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        (void)_cbor_value_extract_int64_helper(it);
        (void)cbor_value_get_int64(it, &s);

        if ((it->flags & CborIteratorFlag_IntegerValueTooLarge) != 0)
            (void)_cbor_value_decode_int64_internal(it);
    } else if (cbor_value_is_tag(it)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(it, &tag);
    } else if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);

        if ((it->flags & CborIteratorFlag_IntegerValueTooLarge) != 0)
            (void)_cbor_value_decode_int64_internal(it);
    } else if ((type == CborArrayType || type == CborMapType) && depth < 16) {
        consume_container(it, depth);
    }
}

static void parse_buffer(const uint8_t *data, size_t size) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(data, size, 0, &parser, &it);
    if (err != CborNoError)
        return;

    consume_value(&it, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    parse_buffer(Data, Size);

    if (Size > 0) {
        size_t split = Data[0] % (Size + 1);
        parse_buffer(Data, split);
    }

    return 0;
}