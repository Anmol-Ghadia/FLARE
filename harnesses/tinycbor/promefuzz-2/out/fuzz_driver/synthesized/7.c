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

int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)Size;
    value.extra = 0;
    value.flags = 0;
    value.type = (Size > 0) ? Data[0] : CborInvalidType;

    parser.source.end = Data + Size;
    parser.flags = 0;

    (void)cbor_value_is_array(&value);
    (void)cbor_value_is_map(&value);
    (void)cbor_value_is_valid(&value);

    if (Size > 1) {
        CborValue v = value;
        v.type = CborArrayType;
        v.flags = Data[1];
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    if (Size > 2) {
        CborValue v = value;
        v.type = CborMapType;
        v.extra = Data[2];
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    if (Size > 3) {
        CborValue v = value;
        v.type = CborInvalidType;
        v.remaining = Data[3];
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    if (Size > 4) {
        CborValue v = value;
        v.type = Data[4];
        if (Size > 6) {
            v.extra = (uint16_t)(Data[5] | ((uint16_t)Data[6] << 8));
        }
        (void)cbor_value_is_array(&v);
        (void)cbor_value_is_map(&v);
        (void)cbor_value_is_valid(&v);
    }

    return 0;
}