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

static void exercise_value(const CborValue *value) {
    size_t length = 0;
    CborType type;
    bool known;
    const uint8_t *next;

    type = cbor_value_get_type(value);
    (void)type;

    if (cbor_value_get_type(value) == CborArrayType) {
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_array_length(value, &length);
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_array_length(value, &length);
    }

    type = cbor_value_get_type(value);
    (void)type;

    if (cbor_value_get_type(value) == CborMapType) {
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_map_length(value, &length);
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_map_length(value, &length);
    }

    if (cbor_value_is_text_string(value) || cbor_value_is_byte_string(value)) {
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_string_length(value, &length);
        (void)cbor_value_get_string_length(value, &length);
    } else {
        (void)cbor_value_is_text_string(value);
        (void)cbor_value_is_byte_string(value);
        (void)cbor_value_is_length_known(value);
    }

    next = cbor_value_get_next_byte(value);
    (void)next;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborError err;

    if (Data == NULL)
        return 0;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    err = cbor_parser_init(Data, Size, 0, &parser, &value);
    if (err == CborNoError) {
        exercise_value(&value);
    }

    if (Size >= 1) {
        CborValue manual;
        memset(&manual, 0, sizeof(manual));
        manual.parser = &parser;
        manual.source.ptr = Data;
        manual.remaining = (uint32_t)(Size > UINT32_MAX ? UINT32_MAX : Size);
        manual.extra = (uint16_t)(Size > UINT16_MAX ? UINT16_MAX : Size);
        manual.flags = (uint8_t)(Data[0] & 0xff);

        manual.type = CborArrayType;
        exercise_value(&manual);

        manual.type = CborMapType;
        exercise_value(&manual);

        manual.type = CborTextStringType;
        exercise_value(&manual);

        manual.type = CborByteStringType;
        exercise_value(&manual);

        manual.type = CborInvalidType;
        exercise_value(&manual);
    }

    return 0;
}