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
    CborValue value;
    CborValue *vp = &value;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    value.parser = &parser;

    if (Size > 0) {
        value.type = Data[0];
    } else {
        value.type = CborInvalidType;
    }

    if (Size > 1) {
        value.flags = Data[1];
    }
    if (Size > 5) {
        value.remaining = ((uint32_t)Data[2] << 24) |
                          ((uint32_t)Data[3] << 16) |
                          ((uint32_t)Data[4] << 8) |
                          (uint32_t)Data[5];
    }
    if (Size > 7) {
        value.extra = (uint16_t)(((uint16_t)Data[6] << 8) | Data[7]);
    }
    if (Size > 8) {
        value.source.ptr = Data + 8;
        parser.source.end = Data + Size;
    } else {
        value.source.ptr = Data;
        parser.source.end = Data + Size;
    }

    (void)cbor_value_is_array(vp);
    (void)cbor_value_is_map(vp);
    (void)cbor_value_is_valid(vp);

    if (Size > 0) {
        CborValue alt = value;

        switch (Data[0] % 4) {
        case 0:
            alt.type = CborArrayType;
            break;
        case 1:
            alt.type = CborMapType;
            break;
        case 2:
            alt.type = CborInvalidType;
            break;
        default:
            alt.type = value.type;
            break;
        }

        (void)cbor_value_is_array(&alt);
        (void)cbor_value_is_map(&alt);
        (void)cbor_value_is_valid(&alt);
    }

    if (Size > 1 && (Data[1] & 1)) {
        vp = NULL;
        (void)cbor_value_is_array(vp);
        (void)cbor_value_is_map(vp);
        (void)cbor_value_is_valid(vp);
    }

    return 0;
}