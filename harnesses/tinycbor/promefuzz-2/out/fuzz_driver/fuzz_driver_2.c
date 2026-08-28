// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
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
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_get_tag at cbor.h:459:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

static void exercise_value(const CborValue *value) {
    if (cbor_value_is_tag(value)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(value, &tag);
    }

    if (cbor_value_is_simple_type(value)) {
        uint8_t simple = 0;
        (void)cbor_value_get_simple_type(value, &simple);
    }

    if (cbor_value_is_boolean(value)) {
        bool b = false;
        (void)cbor_value_get_boolean(value, &b);
    }

    if (cbor_value_is_float(value)) {
        float f = 0.0f;
        (void)cbor_value_get_float(value, &f);
    }

    if (cbor_value_is_double(value)) {
        double d = 0.0;
        (void)cbor_value_get_double(value, &d);
    }

    if (cbor_value_is_half_float(value)) {
        uint16_t hf = 0;
        (void)cbor_value_get_half_float(value, &hf);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        size_t limit = 256;
        while (!cbor_value_at_end(&it) && limit--) {
            exercise_value(&it);

            if (cbor_value_is_container(&it)) {
                CborValue recursed;
                if (cbor_value_enter_container(&it, &recursed) == CborNoError) {
                    size_t inner_limit = 64;
                    while (!cbor_value_at_end(&recursed) && inner_limit--) {
                        exercise_value(&recursed);

                        if (cbor_value_advance(&recursed) != CborNoError)
                            break;
                    }
                    /* Do not call cbor_value_leave_container() unless the
                     * recursed iterator reached CborInvalidType / end state. */
                }
            }

            if (cbor_value_advance(&it) != CborNoError)
                break;
        }
    }

    return 0;
}