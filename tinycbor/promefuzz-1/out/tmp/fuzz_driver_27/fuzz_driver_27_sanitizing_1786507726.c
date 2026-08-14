#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cbor.h"

static int is_valid_value(const CborValue *it) {
    return it && cbor_value_get_type(it) != CborInvalidType;
}

static void consume_value(CborValue *it, int depth);

static void consume_container(CborValue *it, int depth) {
    CborValue rec;
    CborError err;
    int limit = 64;

    if (!is_valid_value(it) || depth > 16)
        return;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError || !is_valid_value(&rec))
        return;

    while (!cbor_value_at_end(&rec) && is_valid_value(&rec) && limit-- > 0) {
        consume_value(&rec, depth + 1);
    }

    if (cbor_value_at_end(&rec))
        (void)cbor_value_leave_container(it, &rec);
}

static void handle_current(const CborValue *it) {
    CborType type;

    if (!is_valid_value(it))
        return;

    type = cbor_value_get_type(it);

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        (void)cbor_value_get_int64(it, &s);
        (void)_cbor_value_extract_int64_helper(it);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
        return;
    }

    if (type == CborTagType && cbor_value_is_tag(it)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(it, &tag);
        return;
    }

    if (type == CborDoubleType && cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
        return;
    }
}

static void consume_value(CborValue *it, int depth) {
    CborType type;
    CborError err;

    if (!is_valid_value(it) || depth > 16)
        return;

    type = cbor_value_get_type(it);
    handle_current(it);

    if (type == CborArrayType || type == CborMapType) {
        consume_container(it, depth);
        return;
    }

    if (type == CborTagType) {
        err = cbor_value_advance_fixed(it);
        if (err != CborNoError || !is_valid_value(it) || cbor_value_at_end(it))
            return;
        consume_value(it, depth + 1);
        return;
    }

    err = cbor_value_advance(it);
    (void)err;
}

static void fuzz_one_mode(const uint8_t *Data, size_t Size, int flags) {
    CborParser parser;
    CborValue it;
    CborError err;
    int limit = 64;

    err = cbor_parser_init(Data, Size, flags, &parser, &it);
    if (err != CborNoError || !is_valid_value(&it))
        return;

    while (!cbor_value_at_end(&it) && is_valid_value(&it) && limit-- > 0) {
        consume_value(&it, 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    fuzz_one_mode(Data, Size, 0);
    fuzz_one_mode(Data, Size, CborValidateBasic);

    return 0;
}