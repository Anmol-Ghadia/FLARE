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

static void exercise_value(const CborValue *it) {
    CborTag tag = 0;
    uint8_t simple = 0;
    bool boolean = false;
    float f = 0.0f;
    double d = 0.0;
    uint16_t half = 0;

    if (cbor_value_is_tag(it)) {
        (void)cbor_value_get_tag(it, &tag);
    }
    if (cbor_value_is_simple_type(it)) {
        (void)cbor_value_get_simple_type(it, &simple);
    }
    if (cbor_value_is_boolean(it)) {
        (void)cbor_value_get_boolean(it, &boolean);
    }
    if (cbor_value_is_float(it)) {
        (void)cbor_value_get_float(it, &f);
    }
    if (cbor_value_is_double(it)) {
        (void)cbor_value_get_double(it, &d);
    }
    if (cbor_value_is_half_float(it)) {
        (void)cbor_value_get_half_float(it, &half);
    }

    (void)tag;
    (void)simple;
    (void)boolean;
    (void)f;
    (void)d;
    (void)half;
}

static void walk_value(CborValue *it, int depth) {
    if (depth > 8) {
        exercise_value(it);
        return;
    }

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        if (cbor_value_enter_container(it, &recursed) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&recursed) && count++ < 32) {
                walk_value(&recursed, depth + 1);
                if (cbor_value_advance(&recursed) != CborNoError) {
                    break;
                }
            }
            (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;

    if (Data == NULL) {
        return 0;
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError) {
        return 0;
    }

    walk_value(&it, 0);

    if (!cbor_value_at_end(&it)) {
        CborValue tmp = it;
        int steps = 0;
        while (!cbor_value_at_end(&tmp) && steps++ < 32) {
            exercise_value(&tmp);
            if (cbor_value_advance(&tmp) != CborNoError) {
                break;
            }
        }
    }

    return 0;
}