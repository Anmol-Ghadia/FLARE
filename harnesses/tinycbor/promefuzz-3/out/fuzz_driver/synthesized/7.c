// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_half_float at cbor.h:599:22 in cbor.h
// cbor_value_is_float at cbor.h:612:22 in cbor.h
// cbor_value_get_float at cbor.h:614:27 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    float f = 0.0f;
    double d = 0.0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    for (;;) {
        (void)cbor_value_is_half_float(&it);

        if (cbor_value_is_float(&it)) {
            (void)cbor_value_get_float(&it, &f);
        }

        if (cbor_value_is_double(&it)) {
            (void)cbor_value_get_double(&it, &d);
        }

        if (cbor_value_at_end(&it))
            break;

        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}