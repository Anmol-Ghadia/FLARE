// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_is_null at cbor.h:385:22 in cbor.h
// cbor_value_is_undefined at cbor.h:387:22 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_get_simple_type at cbor.h:403:27 in cbor.h
// cbor_value_is_boolean at cbor.h:391:22 in cbor.h
// cbor_value_get_boolean at cbor.h:393:27 in cbor.h
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_is_null at cbor.h:385:22 in cbor.h
// cbor_value_is_undefined at cbor.h:387:22 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_is_simple_type at cbor.h:401:22 in cbor.h
// cbor_value_get_simple_type at cbor.h:403:27 in cbor.h
// cbor_value_is_boolean at cbor.h:391:22 in cbor.h
// cbor_value_get_boolean at cbor.h:393:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cbor.h"

static uint16_t read_u16(const uint8_t *data, size_t size, size_t *offset) {
    uint16_t v = 0;
    if (*offset < size) v |= (uint16_t)data[(*offset)++];
    if (*offset < size) v |= (uint16_t)data[(*offset)++] << 8;
    return v;
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    if (*offset < size) v |= (uint32_t)data[(*offset)++];
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 8;
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 16;
    if (*offset < size) v |= (uint32_t)data[(*offset)++] << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t scratch[32];
    size_t to_write = Size < sizeof(scratch) ? Size : sizeof(scratch);
    FILE *fp;

    memcpy(scratch, Data, to_write);
    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (to_write > 0)
            fwrite(scratch, 1, to_write, fp);
        fclose(fp);
    }

    CborParser parser;
    memset(&parser, 0, sizeof(parser));
    parser.source.end = Data + Size;
    parser.flags = 0;

    CborValue value;
    memset(&value, 0, sizeof(value));
    value.parser = &parser;
    value.source.ptr = Data;
    value.remaining = read_u32(Data, Size, &off);
    value.extra = read_u16(Data, Size, &off);
    value.flags = (off < Size) ? Data[off++] : 0;

    {
        uint8_t candidate_types[] = {
            (uint8_t)CborSimpleType,
            (uint8_t)CborNullType,
            (uint8_t)CborUndefinedType,
            (uint8_t)CborDoubleType,
            (uint8_t)CborBooleanType,
            (uint8_t)((off < Size) ? Data[off++] : 0)
        };
        size_t i;

        for (i = 0; i < sizeof(candidate_types) / sizeof(candidate_types[0]); i++) {
            bool b;
            uint8_t simple;
            CborError err;

            value.type = candidate_types[i];

            (void)cbor_value_is_simple_type(&value);
            (void)cbor_value_is_null(&value);
            (void)cbor_value_is_undefined(&value);
            (void)cbor_value_is_double(&value);

            if (cbor_value_is_simple_type(&value)) {
                err = cbor_value_get_simple_type(&value, &simple);
                (void)err;
            }

            if (cbor_value_is_boolean(&value)) {
                err = cbor_value_get_boolean(&value, &b);
                (void)err;
            }
        }
    }

    if (Size > 0) {
        size_t i;
        for (i = 0; i < Size && i < 16; i++) {
            bool b;
            uint8_t simple;
            CborError err;

            value.type = Data[i];
            value.extra = (uint16_t)((i + 1 < Size) ? ((uint16_t)Data[i] | ((uint16_t)Data[i + 1] << 8)) : Data[i]);

            (void)cbor_value_is_simple_type(&value);
            (void)cbor_value_is_null(&value);
            (void)cbor_value_is_undefined(&value);
            (void)cbor_value_is_double(&value);

            if (cbor_value_is_simple_type(&value)) {
                err = cbor_value_get_simple_type(&value, &simple);
                (void)err;
            }

            if (cbor_value_is_boolean(&value)) {
                err = cbor_value_get_boolean(&value, &b);
                (void)err;
            }
        }
    }

    return 0;
}