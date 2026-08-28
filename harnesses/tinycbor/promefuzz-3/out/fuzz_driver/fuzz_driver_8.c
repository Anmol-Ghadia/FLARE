// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    value.parser = &parser;
    value.source.ptr = Data;
    parser.source.end = Data + Size;
    value.type = CborInvalidType;

    if (Size > 0)
        value.type = Data[0];
    if (Size > 1)
        value.flags = Data[1];
    if (Size > 5) {
        value.remaining = ((uint32_t)Data[2] << 24) |
                          ((uint32_t)Data[3] << 16) |
                          ((uint32_t)Data[4] << 8) |
                          (uint32_t)Data[5];
    }
    if (Size > 7)
        value.extra = (uint16_t)(((uint16_t)Data[6] << 8) | Data[7]);
    if (Size > 8)
        value.source.ptr = Data + 8;

    (void)cbor_value_is_array(&value);
    (void)cbor_value_is_map(&value);
    (void)cbor_value_is_valid(&value);

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
            break;
        }

        (void)cbor_value_is_array(&alt);
        (void)cbor_value_is_map(&alt);
        (void)cbor_value_is_valid(&alt);
    }

    if (Size > 1) {
        CborValue zeroed;
        memset(&zeroed, 0, sizeof(zeroed));
        zeroed.type = (Data[1] & 1) ? CborInvalidType : value.type;

        (void)cbor_value_is_array(&zeroed);
        (void)cbor_value_is_map(&zeroed);
        (void)cbor_value_is_valid(&zeroed);
    }

    return 0;
}