#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static void consume_value(CborValue *it);

static int is_fixed_type_safe(CborType type) {
    switch (type) {
    case CborIntegerType:
    case CborByteStringType:
    case CborTextStringType:
    case CborTagType:
    case CborSimpleType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
        return 1;
    default:
        return 0;
    }
}

static void handle_scalar(CborValue *it) {
    CborType type = cbor_value_get_type(it);
    CborError err;

    if (type == CborInvalidType)
        return;

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        err = cbor_value_get_int64(it, &s);
        if (err != CborNoError)
            return;
        (void)_cbor_value_extract_int64_helper(it);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
    } else if (type == CborTagType && cbor_value_is_tag(it)) {
        CborTag tag = 0;
        err = cbor_value_get_tag(it, &tag);
        if (err != CborNoError)
            return;
    } else if (type == CborDoubleType && cbor_value_is_double(it)) {
        double d = 0.0;
        err = cbor_value_get_double(it, &d);
        if (err != CborNoError)
            return;
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
    }

    type = cbor_value_get_type(it);
    if (type == CborInvalidType || !is_fixed_type_safe(type))
        return;

    (void)cbor_value_advance_fixed(it);
}

static void consume_container(CborValue *it) {
    CborValue rec;
    if (cbor_value_enter_container(it, &rec) != CborNoError)
        return;

    int limit = 64;
    while (!cbor_value_at_end(&rec) && limit-- > 0) {
        if (cbor_value_get_type(&rec) == CborInvalidType)
            break;
        consume_value(&rec);
    }

    (void)cbor_value_leave_container(it, &rec);
}

static void consume_value(CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (type == CborInvalidType)
        return;

    switch (type) {
    case CborArrayType:
    case CborMapType:
        consume_container(it);
        break;
    case CborTagType: {
        CborTag tag = 0;
        if (cbor_value_is_tag(it)) {
            if (cbor_value_get_tag(it, &tag) != CborNoError)
                return;
        }
        if (cbor_value_get_type(it) == CborInvalidType)
            return;
        if (cbor_value_advance_fixed(it) == CborNoError && !cbor_value_at_end(it))
            consume_value(it);
        break;
    }
    default:
        handle_scalar(it);
        break;
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
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        int top_limit = 64;
        while (!cbor_value_at_end(&it) && top_limit-- > 0) {
            if (cbor_value_get_type(&it) == CborInvalidType)
                break;
            consume_value(&it);
        }
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        int top_limit = 64;
        while (!cbor_value_at_end(&it) && top_limit-- > 0) {
            if (cbor_value_get_type(&it) == CborInvalidType)
                break;
            consume_value(&it);
        }
    }

    return 0;
}