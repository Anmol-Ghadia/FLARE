#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "cbor.h"

static void process_value(CborValue *it, int depth);

static void process_container(CborValue *it, int depth) {
    CborError err;
    CborValue recursed;

    if (depth > 32)
        return;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        process_value(&recursed, depth + 1);

        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void process_value(CborValue *it, int depth) {
    uint8_t simple = 0;
    float f = 0.0f;
    double d = 0.0;
    CborError err;

    if (cbor_value_is_simple_type(it)) {
        err = cbor_value_get_simple_type(it, &simple);
        (void)err;
    }

    if (cbor_value_is_float(it)) {
        err = cbor_value_get_float(it, &f);
        (void)err;
    }

    if (cbor_value_is_double(it)) {
        err = cbor_value_get_double(it, &d);
        (void)err;
    }

    if (cbor_value_is_half_float(it)) {
        err = cbor_value_get_half_float_as_float(it, &f);
        (void)err;
    }

    if (depth > 32)
        return;

    if (cbor_value_is_array(it) || cbor_value_is_map(it)) {
        process_container(it, depth);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    process_value(&it, 0);

    return 0;
}