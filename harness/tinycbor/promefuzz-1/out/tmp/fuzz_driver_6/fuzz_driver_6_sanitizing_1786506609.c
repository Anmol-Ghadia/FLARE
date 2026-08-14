#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_value(CborValue *value) {
    bool is_half;
    bool is_float;
    bool is_double;
    float f = 0.0f;
    double d = 0.0;

    is_half = cbor_value_is_half_float(value);
    is_float = cbor_value_is_float(value);
    if (is_float) {
        (void)cbor_value_get_float(value, &f);
    }

    is_double = cbor_value_is_double(value);
    if (is_double) {
        (void)cbor_value_get_double(value, &d);
    }

    if (is_half) {
        volatile bool sink_b = is_half;
        (void)sink_b;
    }
    if (is_float) {
        volatile float sink_f = f;
        (void)sink_f;
    }
    if (is_double) {
        volatile double sink_d = d;
        (void)sink_d;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    while (!cbor_value_at_end(&it)) {
        exercise_value(&it);

        if (cbor_value_is_container(&it)) {
            CborValue recursed;
            err = cbor_value_enter_container(&it, &recursed);
            if (err == CborNoError) {
                while (!cbor_value_at_end(&recursed)) {
                    exercise_value(&recursed);
                    err = cbor_value_advance_fixed(&recursed);
                    if (err != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&it, &recursed);
            } else {
                err = cbor_value_advance_fixed(&it);
                if (err != CborNoError)
                    break;
            }
        } else {
            err = cbor_value_advance_fixed(&it);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}