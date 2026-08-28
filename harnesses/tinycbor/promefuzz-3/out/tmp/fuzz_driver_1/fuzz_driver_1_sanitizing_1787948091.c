#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    memset(&parser, 0, sizeof(parser));
    parser.source.end = Data + Size;

    CborValue value;
    memset(&value, 0, sizeof(value));
    value.parser = &parser;
    value.source.ptr = Data;

    if (Size > 0) {
        value.type = Data[0];
    } else {
        value.type = CborInvalidType;
    }

    if (Size >= 5) {
        value.remaining = ((uint32_t)Data[1] << 24) |
                          ((uint32_t)Data[2] << 16) |
                          ((uint32_t)Data[3] << 8) |
                          (uint32_t)Data[4];
    } else {
        value.remaining = 0;
    }

    if (Size >= 7) {
        value.extra = (uint16_t)(((uint16_t)Data[5] << 8) | Data[6]);
    } else {
        value.extra = 0;
    }

    if (Size >= 8) {
        value.flags = Data[7];
    } else {
        value.flags = 0;
    }

    (void)cbor_value_is_array(&value);

    bool is_bs = cbor_value_is_byte_string(&value);
    (void)cbor_value_is_text_string(&value);

    if (is_bs || cbor_value_is_text_string(&value)) {
        CborValue iter = value;
        (void)cbor_value_begin_string_iteration(&iter);
    }

    is_bs = cbor_value_is_byte_string(&value);

    if (is_bs) {
        uint8_t smallbuf[32];
        size_t smalllen = sizeof(smallbuf);
        CborValue next1;
        memset(&next1, 0, sizeof(next1));
        (void)cbor_value_copy_byte_string(&value, smallbuf, &smalllen, &next1);

        size_t dynlen = Size;
        if (dynlen > 4096)
            dynlen = 4096;
        uint8_t *dynbuf = dynlen ? (uint8_t *)malloc(dynlen) : NULL;
        CborValue next2;
        memset(&next2, 0, sizeof(next2));
        (void)cbor_value_copy_byte_string(&value, dynbuf, &dynlen, &next2);
        free(dynbuf);
    }

    (void)cbor_value_at_end(&value);
    (void)cbor_value_get_type(&value);

    if (is_bs || cbor_value_is_text_string(&value)) {
        size_t length = 0;
        (void)cbor_value_get_string_length(&value, &length);
    }

    (void)cbor_value_at_end(&value);
    (void)cbor_value_get_next_byte(&value);

    return 0;
}