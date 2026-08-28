#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
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