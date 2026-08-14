// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_9(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborValue next;
    const uint8_t *byte_buf = NULL;
    const char *text_buf = NULL;
    size_t len = 0;
    CborError err;
    CborType type;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    memset(&next, 0, sizeof(next));

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)Size;
    value.extra = 0;
    value.flags = 0;

    if (Size == 0) {
        value.type = CborInvalidType;
        type = cbor_value_get_type(&value);
        (void)type;
        return 0;
    }

    if ((Data[0] & 0xe0) == CborByteStringType) {
        value.type = CborByteStringType;
    } else if ((Data[0] & 0xe0) == CborTextStringType) {
        value.type = CborTextStringType;
    } else {
        value.type = (uint8_t)(Data[0]);
    }

    type = cbor_value_get_type(&value);

    if (type == CborByteStringType) {
        err = cbor_value_get_byte_string_chunk(&value, &byte_buf, &len, &next);
        (void)err;
    } else {
        CborValue safe_byte = value;
        static const uint8_t kByteString[] = { 0x41, 0x00 };
        CborParser safe_parser;
        memset(&safe_parser, 0, sizeof(safe_parser));
        safe_parser.source.end = kByteString + sizeof(kByteString);
        safe_parser.flags = 0;
        safe_byte.parser = &safe_parser;
        safe_byte.source.ptr = kByteString;
        safe_byte.remaining = 1;
        safe_byte.extra = 1;
        safe_byte.type = CborByteStringType;
        safe_byte.flags = 0;
        err = cbor_value_get_byte_string_chunk(&safe_byte, &byte_buf, &len, &next);
        (void)err;
    }

    if (type == CborTextStringType) {
        err = cbor_value_get_text_string_chunk(&value, &text_buf, &len, &next);
        (void)err;
    } else {
        CborValue safe_text = value;
        static const uint8_t kTextString[] = { 0x61, 'A' };
        CborParser safe_parser;
        memset(&safe_parser, 0, sizeof(safe_parser));
        safe_parser.source.end = kTextString + sizeof(kTextString);
        safe_parser.flags = 0;
        safe_text.parser = &safe_parser;
        safe_text.source.ptr = kTextString;
        safe_text.remaining = 1;
        safe_text.extra = 1;
        safe_text.type = CborTextStringType;
        safe_text.flags = 0;
        err = cbor_value_get_text_string_chunk(&safe_text, &text_buf, &len, &next);
        (void)err;
    }

    (void)byte_buf;
    (void)text_buf;
    return 0;
}