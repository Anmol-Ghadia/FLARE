// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_is_float at cbor.h:612:22 in cbor.h
// cbor_value_get_float at cbor.h:614:27 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
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

static void consume_container(CborValue *container) {
    CborValue recursed;
    if (container == NULL)
        return;
    if (!cbor_value_is_container(container))
        return;

    if (cbor_value_enter_container(container, &recursed) != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        exercise_value(&recursed);

        if (cbor_value_is_container(&recursed)) {
            consume_container(&recursed);
        } else {
            if (cbor_value_advance(&recursed) != CborNoError)
                break;
        }
    }

    if (cbor_value_at_end(&recursed)) {
        (void)cbor_value_leave_container(container, &recursed);
    }
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
            consume_container(&it);
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