#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue value;
    CborValue next1, next2;
    const uint8_t *byte_buf = NULL;
    const char *text_buf = NULL;
    size_t byte_len = 0;
    size_t text_len = 0;

    memset(&parser, 0, sizeof(parser));
    memset(&value, 0, sizeof(value));
    memset(&next1, 0, sizeof(next1));
    memset(&next2, 0, sizeof(next2));

    parser.source.end = Data + Size;
    parser.flags = 0;

    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = (uint32_t)Size;
    value.extra = (Size > 0) ? Data[0] : 0;
    value.flags = (Size > 1) ? Data[1] : 0;

    {
        uint8_t selector = (Size > 2) ? Data[2] : 0;
        switch (selector % 6) {
        case 0:
            value.type = CborByteStringType;
            break;
        case 1:
            value.type = CborTextStringType;
            break;
        case 2:
            value.type = CborInvalidType;
            break;
        case 3:
            value.type = CborArrayType;
            break;
        case 4:
            value.type = CborMapType;
            break;
        default:
            value.type = (Size > 3) ? Data[3] : CborByteStringType;
            break;
        }
    }

    {
        CborType t = cbor_value_get_type(&value);

        if (t == CborByteStringType) {
            (void)cbor_value_get_byte_string_chunk(&value, &byte_buf, &byte_len, &next1);
        } else {
            CborValue forced = value;
            forced.type = CborByteStringType;
            (void)cbor_value_get_byte_string_chunk(&forced, &byte_buf, &byte_len, &next1);
        }

        if (t == CborTextStringType) {
            (void)cbor_value_get_text_string_chunk(&value, &text_buf, &text_len, &next2);
        } else {
            CborValue forced = value;
            forced.type = CborTextStringType;
            (void)cbor_value_get_text_string_chunk(&forced, &text_buf, &text_len, &next2);
        }
    }

    if (Size >= sizeof(CborValue)) {
        CborValue crafted;
        memset(&crafted, 0, sizeof(crafted));
        memcpy(&crafted, Data, sizeof(crafted));
        crafted.parser = &parser;
        if (crafted.source.ptr == NULL)
            crafted.source.ptr = Data;

        {
            CborType t = cbor_value_get_type(&crafted);

            if (t == CborByteStringType) {
                (void)cbor_value_get_byte_string_chunk(&crafted, &byte_buf, &byte_len, &next1);
            } else {
                CborValue forced = crafted;
                forced.type = CborByteStringType;
                (void)cbor_value_get_byte_string_chunk(&forced, &byte_buf, &byte_len, &next1);
            }

            if (t == CborTextStringType) {
                (void)cbor_value_get_text_string_chunk(&crafted, &text_buf, &text_len, &next2);
            } else {
                CborValue forced = crafted;
                forced.type = CborTextStringType;
                (void)cbor_value_get_text_string_chunk(&forced, &text_buf, &text_len, &next2);
            }
        }
    }

    (void)byte_buf;
    (void)text_buf;
    (void)byte_len;
    (void)text_len;
    return 0;
}