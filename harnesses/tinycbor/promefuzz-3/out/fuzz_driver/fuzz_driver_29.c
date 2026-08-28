// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_float at cbor.h:612:22 in cbor.h
// cbor_value_get_float at cbor.h:614:27 in cbor.h
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_get_half_float at cbor.h:602:27 in cbor.h
// cbor_value_get_half_float_as_float at cborparser_float.c:40:11 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_float_apis(const CborValue *it) {
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

static void walk_value(const CborValue *it, size_t depth) {
    CborError err;

    if (depth > 16)
        return;

    exercise_float_apis(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        size_t limit = 0;

        err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&recursed) && limit++ < 128) {
            CborValue current = recursed;

            walk_value(&current, depth + 1);

            err = cbor_value_advance(&recursed);
            if (err != CborNoError)
                break;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (cbor_value_at_end(&it))
        return 0;

    walk_value(&it, 0);
    return 0;
}