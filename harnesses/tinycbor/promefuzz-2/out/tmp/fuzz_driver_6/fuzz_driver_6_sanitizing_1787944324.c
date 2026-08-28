#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_value(const CborValue *value) {
    float f = 0.0f;
    double d = 0.0;
    bool b1, b2, b3;
    CborError err1, err2;

    if (value == NULL)
        return;

    b1 = cbor_value_is_half_float(value);
    b2 = cbor_value_is_float(value);
    if (b2) {
        err1 = cbor_value_get_float(value, &f);
        (void)err1;
    }
    b3 = cbor_value_is_double(value);
    if (b3) {
        err2 = cbor_value_get_double(value, &d);
        (void)err2;
    }

    (void)b1;
    (void)b2;
    (void)b3;
    (void)f;
    (void)d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        exercise_value(&it);

        if (cbor_value_is_container(&it)) {
            CborValue recursed;
            if (cbor_value_enter_container(&it, &recursed) == CborNoError) {
                exercise_value(&recursed);
                (void)cbor_value_leave_container(&it, &recursed);
            }
        }

        if (!cbor_value_at_end(&it)) {
            CborValue next = it;
            if (cbor_value_is_valid(&next) && !cbor_value_is_container(&next) &&
                !cbor_value_is_byte_string(&next) && !cbor_value_is_text_string(&next) &&
                !cbor_value_is_tag(&next)) {
                if (cbor_value_advance_fixed(&next) == CborNoError) {
                    exercise_value(&next);
                }
            } else {
                next = it;
                if (cbor_value_advance(&next) == CborNoError) {
                    exercise_value(&next);
                }
            }
        }
    }

    return 0;
}