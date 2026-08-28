#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void consume_value(CborValue *it);

static void handle_current_value(CborValue *it) {
    float f = 0.0f;
    uint16_t half_bits = 0;

    if (cbor_value_is_float(it)) {
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_half_float(it)) {
        (void)cbor_value_get_half_float(it, &half_bits);
        (void)cbor_value_get_half_float_as_float(it, &f);
    }

    if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
    }
}

static void consume_container(CborValue *it) {
    CborValue recursed;
    CborError err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    size_t limit = 0;
    while (!cbor_value_at_end(&recursed) && limit++ < 128) {
        consume_value(&recursed);
        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void consume_value(CborValue *it) {
    handle_current_value(it);

    if (cbor_value_is_container(it)) {
        consume_container(it);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    size_t top_limit = 0;
    while (!cbor_value_at_end(&it) && top_limit++ < 128) {
        consume_value(&it);
        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}