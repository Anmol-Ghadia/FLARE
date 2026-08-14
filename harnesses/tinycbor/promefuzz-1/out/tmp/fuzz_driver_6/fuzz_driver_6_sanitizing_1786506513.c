#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_value(CborValue *value) {
    bool is_half = cbor_value_is_half_float(value);
    bool is_float = cbor_value_is_float(value);
    float f = 0.0f;
    bool is_double;
    double d = 0.0;

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
    CborValue value;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &value);
    if (err != CborNoError)
        return 0;

    while (!cbor_value_at_end(&value)) {
        exercise_value(&value);

        if (cbor_value_is_container(&value)) {
            CborValue recursed;
            if (cbor_value_enter_container(&value, &recursed) == CborNoError) {
                while (!cbor_value_at_end(&recursed)) {
                    exercise_value(&recursed);
                    if (cbor_value_advance(&recursed) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&value, &recursed);
                continue;
            }
        }

        if (cbor_value_advance(&value) != CborNoError)
            break;
    }

    return 0;
}