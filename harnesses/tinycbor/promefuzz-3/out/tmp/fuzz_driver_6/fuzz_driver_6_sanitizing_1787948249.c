#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "cbor.h"

static void init_value_from_data(CborParser *parser, CborValue *value,
                                 const uint8_t *Data, size_t Size)
{
    memset(parser, 0, sizeof(*parser));
    memset(value, 0, sizeof(*value));

    value->parser = parser;
    value->source.ptr = Data;
    value->remaining = (uint32_t)Size;

    if (Size > 0)
        value->type = Data[0];
    if (Size > 1)
        value->flags = Data[1];
    if (Size > 3)
        value->extra = (uint16_t)(Data[2] | ((uint16_t)Data[3] << 8));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue value;
    float f = 0.0f;
    double d = 0.0;

    init_value_from_data(&parser, &value, Data, Size);

    (void)cbor_value_is_half_float(&value);

    if (cbor_value_is_float(&value) &&
        (value.flags & CborIteratorFlag_IntegerValueTooLarge) &&
        value.source.ptr != NULL &&
        Size >= 5) {
        (void)cbor_value_get_float(&value, &f);
    }

    if (cbor_value_is_double(&value) &&
        (value.flags & CborIteratorFlag_IntegerValueTooLarge) &&
        value.source.ptr != NULL &&
        Size >= 9) {
        (void)cbor_value_get_double(&value, &d);
    }

    if (Size >= 2) {
        init_value_from_data(&parser, &value, Data, Size);
        value.type = CborHalfFloatType;

        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        (void)cbor_value_is_double(&value);
    }

    if (Size >= 5) {
        init_value_from_data(&parser, &value, Data, Size);
        value.type = CborFloatType;
        value.flags |= CborIteratorFlag_IntegerValueTooLarge;

        (void)cbor_value_is_half_float(&value);
        if (cbor_value_is_float(&value) &&
            (value.flags & CborIteratorFlag_IntegerValueTooLarge) &&
            value.source.ptr != NULL) {
            (void)cbor_value_get_float(&value, &f);
        }
        (void)cbor_value_is_double(&value);
    }

    if (Size >= 9) {
        init_value_from_data(&parser, &value, Data, Size);
        value.type = CborDoubleType;
        value.flags |= CborIteratorFlag_IntegerValueTooLarge;

        (void)cbor_value_is_half_float(&value);
        (void)cbor_value_is_float(&value);
        if (cbor_value_is_double(&value) &&
            (value.flags & CborIteratorFlag_IntegerValueTooLarge) &&
            value.source.ptr != NULL) {
            (void)cbor_value_get_double(&value, &d);
        }
    }

    (void)f;
    (void)d;
    return 0;
}