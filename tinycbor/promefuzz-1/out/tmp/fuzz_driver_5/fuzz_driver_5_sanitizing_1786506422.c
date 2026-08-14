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
    (void)type;

    bool is_container = cbor_value_is_container(value);
    (void)is_container;

    if (type == CborIntegerType) {
        int64_t result = 0;
        (void)cbor_value_get_int64(value, &result);
    }

    if (type == CborByteStringType) {
        uint8_t *buffer = NULL;
        size_t buflen = 0;
        CborValue next = *value;
        CborError err = cbor_value_dup_byte_string(value, &buffer, &buflen, &next);
        if (err == CborNoError) {
            free(buffer);
        }
    }

    if (type == CborTextStringType) {
        char *buffer = NULL;
        size_t buflen = 0;
        CborValue next = *value;
        CborError err = cbor_value_dup_text_string(value, &buffer, &buflen, &next);
        if (err == CborNoError) {
            free(buffer);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        exercise_value(&it);
    }

    if (Size >= sizeof(CborValue)) {
        CborValue synthetic;
        memset(&synthetic, 0, sizeof(synthetic));

        memcpy(&synthetic, Data, sizeof(CborValue));

        {
            CborParser local_parser;
            memset(&local_parser, 0, sizeof(local_parser));
            local_parser.source.end = Data + Size;
            synthetic.parser = &local_parser;
            exercise_value(&synthetic);
        }
    }

    if (Size > 0) {
        uint8_t *copy = (uint8_t *)malloc(Size);
        if (copy) {
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