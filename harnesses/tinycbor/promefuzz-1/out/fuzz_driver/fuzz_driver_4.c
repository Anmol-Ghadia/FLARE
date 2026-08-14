// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_array_length at cbor.h:570:27 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_array_length at cbor.h:570:27 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_map_length at cbor.h:583:27 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_map_length at cbor.h:583:27 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_get_string_length at cbor.h:473:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_next_byte at cbor.h:362:32 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
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

    known = cbor_value_is_length_known(value);
    (void)known;

    if (type == CborArrayType) {
        (void)cbor_value_get_array_length(value, &length);
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_array_length(value, &length);
    }

    type = cbor_value_get_type(value);
    (void)type;

    known = cbor_value_is_length_known(value);
    (void)known;

    if (type == CborMapType) {
        (void)cbor_value_get_map_length(value, &length);
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_map_length(value, &length);
    }

    if (cbor_value_is_text_string(value)) {
        (void)cbor_value_is_byte_string(value);
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_string_length(value, &length);
        (void)cbor_value_get_string_length(value, &length);
    } else if (cbor_value_is_byte_string(value)) {
        known = cbor_value_is_length_known(value);
        (void)known;
        (void)cbor_value_get_string_length(value, &length);
        (void)cbor_value_get_string_length(value, &length);
    } else {
        (void)cbor_value_is_byte_string(value);
        known = cbor_value_is_length_known(value);
        (void)known;
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

    return 0;
}