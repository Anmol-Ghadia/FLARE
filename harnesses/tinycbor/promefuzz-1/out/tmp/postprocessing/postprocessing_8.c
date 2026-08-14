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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    memset(&parser, 0, sizeof(parser));

    CborValue value;
    memset(&value, 0, sizeof(value));

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)(Size & 0xffffffffu);

    if (Size > 0) {
        value.type = Data[0];
    } else {
        value.type = CborInvalidType;
    }

    if (Size > 1) {
        value.flags = Data[1];
    }
    if (Size > 3) {
        value.extra = (uint16_t)((Data[2] << 8) | Data[3]);
    }

    (void)cbor_value_is_array(&value);
    (void)cbor_value_is_map(&value);
    (void)cbor_value_is_valid(&value);

    if (Size > 0) {
        CborValue mutated = value;

        switch (Data[0] % 4) {
        case 0:
            mutated.type = CborArrayType;
            break;
        case 1:
            mutated.type = CborMapType;
            break;
        case 2:
            mutated.type = CborInvalidType;
            break;
        default:
            mutated.type = (uint8_t)(Data[0] ^ 0xFF);
            break;
        }

        (void)cbor_value_is_array(&mutated);
        (void)cbor_value_is_map(&mutated);
        (void)cbor_value_is_valid(&mutated);
    }

    if (Size > 4) {
        CborValue null_parser_value = value;
        null_parser_value.parser = NULL;

        (void)cbor_value_is_array(&null_parser_value);
        (void)cbor_value_is_map(&null_parser_value);
        (void)cbor_value_is_valid(&null_parser_value);
    }

    return 0;
}