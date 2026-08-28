// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_dup_byte_string at cbor.h:512:27 in cbor.h
// cbor_value_dup_text_string at cbor.h:506:27 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

static void exercise_value(const CborValue *value) {
    bool at_end = cbor_value_at_end(value);
    (void)at_end;

    CborType type = cbor_value_get_type(value);
    bool is_container = cbor_value_is_container(value);
    (void)is_container;

    if (type == CborIntegerType) {
        int64_t result = 0;
        (void)cbor_value_get_int64(value, &result);
    }

    if (type == CborByteStringType) {
        uint8_t *buffer = NULL;
        size_t buflen = 0;
        CborValue next;
        if (cbor_value_dup_byte_string(value, &buffer, &buflen, &next) == CborNoError) {
            free(buffer);
            exercise_value(&next);
        }
    }

    if (type == CborTextStringType) {
        char *buffer = NULL;
        size_t buflen = 0;
        CborValue next;
        if (cbor_value_dup_text_string(value, &buffer, &buflen, &next) == CborNoError) {
            free(buffer);
            exercise_value(&next);
        }
    }
}

int LLVMFuzzerTestOneInput_5(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    exercise_value(&it);

    if (Size > 0) {
        uint8_t *copy = (uint8_t *)malloc(Size);
        if (copy != NULL) {
            memcpy(copy, Data, Size);
            err = cbor_parser_init(copy, Size, 0, &parser, &it);
            if (err == CborNoError) {
                exercise_value(&it);
            }
            free(copy);
        }
    }

    return 0;
}