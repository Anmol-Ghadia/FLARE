#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
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
        value.flags = Data[1];
    if (Size > 3)
        value.extra = (uint16_t)((uint16_t)Data[2] | ((uint16_t)Data[3] << 8));
    if (Size > 7) {
        value.remaining = (uint32_t)Data[4] |
                          ((uint32_t)Data[5] << 8) |
                          ((uint32_t)Data[6] << 16) |
                          ((uint32_t)Data[7] << 24);
    }

    (void)cbor_value_is_half_float(&value);

    if (cbor_value_is_float(&value) &&
        !(value.flags & CborIteratorFlag_IntegerValueTooLarge)) {
        (void)cbor_value_get_float(&value, &f);
    }

    if (cbor_value_is_double(&value) &&
        (value.flags & CborIteratorFlag_IntegerValueTooLarge)) {
        (void)cbor_value_get_double(&value, &d);
    }

    if (Size > 0) {
        memset(&value, 0, sizeof(value));
        value.parser = &parser;
        value.type = CborHalfFloatType;
        if (Size > 1)
            value.flags = Data[1];
        if (Size > 3)
            value.extra = (uint16_t)((uint16_t)Data[2] | ((uint16_t)Data[3] << 8));

        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        (void)cbor_value_is_double(&value);
    }

    if (Size > 0) {
        memset(&value, 0, sizeof(value));
        value.parser = &parser;
        value.type = CborFloatType;
        if (Size > 1)
            value.flags = (uint8_t)(Data[1] & (uint8_t)~CborIteratorFlag_IntegerValueTooLarge);
        if (Size > 5) {
            value.extra = (uint16_t)((uint16_t)Data[2] | ((uint16_t)Data[3] << 8));
            value.remaining = (uint32_t)Data[4] |
                              ((uint32_t)Data[5] << 8);
        }

        (void)cbor_value_is_half_float(&value);
        if (cbor_value_is_float(&value) &&
            !(value.flags & CborIteratorFlag_IntegerValueTooLarge)) {
            (void)cbor_value_get_float(&value, &f);
        }
        (void)cbor_value_is_double(&value);
    }

    if (Size > 0) {
        memset(&value, 0, sizeof(value));
        value.parser = &parser;
        value.type = CborDoubleType;
        value.flags = CborIteratorFlag_IntegerValueTooLarge;
        if (Size > 3)
            value.extra = (uint16_t)((uint16_t)Data[1] | ((uint16_t)Data[2] << 8));
        if (Size > 7) {
            value.remaining = (uint32_t)Data[3] |
                              ((uint32_t)Data[4] << 8) |
                              ((uint32_t)Data[5] << 16) |
                              ((uint32_t)Data[6] << 24);
        }

        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        if (cbor_value_is_double(&value) &&
            (value.flags & CborIteratorFlag_IntegerValueTooLarge)) {
            (void)cbor_value_get_double(&value, &d);
        }
    }

    (void)f;
    (void)d;
    return 0;
}