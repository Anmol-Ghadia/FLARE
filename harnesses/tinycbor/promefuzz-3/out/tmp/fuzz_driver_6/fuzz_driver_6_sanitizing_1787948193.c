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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue value;
    float f = 0.0f;
    double d = 0.0;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    value.parser = &parser;

    if (Size > 0)
        value.type = Data[0];
    if (Size > 1)
        value.extra = (uint16_t)Data[1];
    if (Size > 5) {
        value.extra = (uint16_t)((uint16_t)Data[1] | ((uint16_t)Data[2] << 8));
        value.remaining = (uint32_t)Data[3] |
                          ((uint32_t)Data[4] << 8) |
                          ((uint32_t)Data[5] << 16);
    }

    if (Size > 0) {
        bool is_half = cbor_value_is_half_float(&value);
        bool is_float = cbor_value_is_float(&value);

        if (is_float) {
            (void)cbor_value_get_float(&value, &f);
        }

        bool is_double = cbor_value_is_double(&value);
        if (is_double) {
            (void)cbor_value_get_double(&value, &d);
        }

        (void)is_half;
    }

    if (Size >= 4) {
        memset(&value, 0, sizeof(value));
        value.parser = &parser;
        value.type = Data[0];
        value.extra = (uint16_t)((uint16_t)Data[1] | ((uint16_t)Data[2] << 8));
        value.remaining = (uint32_t)Data[3];

        bool is_half = cbor_value_is_half_float(&value);
        bool is_float = cbor_value_is_float(&value);

        if (is_float) {
            (void)cbor_value_get_float(&value, &f);
        }

        bool is_double = cbor_value_is_double(&value);
        if (is_double) {
            (void)cbor_value_get_double(&value, &d);
        }

        (void)is_half;
    }

    if (Size >= 1) {
        memset(&value, 0, sizeof(value));
        value.parser = &parser;

        value.type = CborHalfFloatType;
        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        (void)cbor_value_is_double(&value);

        value.type = CborFloatType;
        (void)cbor_value_is_half_float(&value);
        if (cbor_value_is_float(&value)) {
            (void)cbor_value_get_float(&value, &f);
        }
        (void)cbor_value_is_double(&value);

        value.type = CborDoubleType;
        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        if (cbor_value_is_double(&value)) {
            (void)cbor_value_get_double(&value, &d);
        }
    }

    return 0;
}