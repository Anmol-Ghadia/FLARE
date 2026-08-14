#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cbor.h"

static void consume_value(CborValue *it);

static void advance_safely(CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (type == CborArrayType || type == CborMapType) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) != CborNoError)
            return;

        int limit = 64;
        while (!cbor_value_at_end(&rec) && limit-- > 0) {
            consume_value(&rec);
        }

        if (cbor_value_at_end(&rec))
            (void)cbor_value_leave_container(it, &rec);
        return;
    }

    (void)cbor_value_advance(it);
}

static void handle_current(CborValue *it) {
    CborType type = cbor_value_get_type(it);

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

static void consume_value(CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (type == CborTagType) {
        if (cbor_value_is_tag(it)) {
            CborTag tag = 0;
            (void)cbor_value_get_tag(it, &tag);
        }
        if (cbor_value_advance_fixed(it) != CborNoError)
            return;
        if (!cbor_value_at_end(it))
            consume_value(it);
        return;
    }

    handle_current(it);
    advance_safely(it);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        CborParser parser;
        CborValue it;
        CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
        if (err == CborNoError) {
            int limit = 64;
            while (!cbor_value_at_end(&it) && limit-- > 0)
                consume_value(&it);
        }
    }

    {
        CborParser parser;
        CborValue it;
        CborError err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
        if (err == CborNoError) {
            int limit = 64;
            while (!cbor_value_at_end(&it) && limit-- > 0)
                consume_value(&it);
        }
    }

    return 0;
}