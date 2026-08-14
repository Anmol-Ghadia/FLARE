// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_get_tag at cbor.h:459:27 in cbor.h
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_get_simple_type at cbor.h:403:27 in cbor.h
// cbor_value_is_boolean at cbor.h:391:22 in cbor.h
// cbor_value_get_boolean at cbor.h:393:27 in cbor.h
// cbor_value_is_float at cbor.h:612:22 in cbor.h
// cbor_value_get_float at cbor.h:614:27 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_get_half_float at cbor.h:602:27 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
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

    if (!cbor_value_is_container(it)) {
        return;
    }

    CborValue recursed;
    CborError err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError) {
        return;
    }

    while (!cbor_value_at_end(&recursed)) {
        CborValue current = recursed;
        walk_value(&current, depth + 1);

        err = cbor_value_advance(&recursed);
        if (err != CborNoError) {
            return;
        }
    }

    (void)cbor_value_leave_container(it, &recursed);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL) {
        return 0;
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    walk_value(&it, 0);
    return 0;
}