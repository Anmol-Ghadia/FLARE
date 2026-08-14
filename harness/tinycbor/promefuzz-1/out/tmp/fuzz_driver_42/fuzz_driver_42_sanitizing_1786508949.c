#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

static void consume_value(CborValue *it, int depth);

static void handle_current_value(const CborValue *it) {
    CborType t = cbor_value_get_type(it);

    if (cbor_value_is_simple_type(it)) {
        uint8_t simple = 0;
        (void)cbor_value_get_simple_type(it, &simple);
    }

    if (cbor_value_is_float(it)) {
        float f = 0.0f;
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
    }

    if (t == CborHalfFloatType) {
        float hf = 0.0f;
        (void)cbor_value_get_half_float_as_float(it, &hf);
    }
}

static void consume_container(CborValue *it, int depth) {
    CborValue recursed;
    CborError err;
    int limit = 0;

    if (depth > 16)
        return;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed) && limit++ < 64) {
        consume_value(&recursed, depth + 1);
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void consume_value(CborValue *it, int depth) {
    CborType t;
    CborError err;

    if (cbor_value_at_end(it))
        return;

    t = cbor_value_get_type(it);
    if (t == CborInvalidType)
        return;

    handle_current_value(it);

    if (t == CborArrayType || t == CborMapType) {
        consume_container(it, depth);
        return;
    }

    err = cbor_value_advance_fixed(it);
    if (err != CborNoError) {
        (void)cbor_value_advance(it);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        consume_value(&it, 0);
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        consume_value(&it, 0);
    }

    return 0;
}