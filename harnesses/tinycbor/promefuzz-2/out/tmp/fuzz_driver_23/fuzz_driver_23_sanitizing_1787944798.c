#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static uint16_t read_u16(const uint8_t *p) {
    uint16_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static uint32_t read_u32(const uint8_t *p) {
    uint32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static uint64_t read_u64(const uint8_t *p) {
    uint64_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
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
    value.remaining = (uint32_t)Size;
    value.flags = 0;

    float f_res = 0.0f;
    double d_res = 0.0;
    bool b_res = false;
    CborError err;

    if (Size >= 1) {
        uint8_t selector = Data[0] % 4;

        if (selector == 0) {
            value.type = CborHalfFloatType;
            if (Size >= 3)
                value.extra = read_u16(Data + 1);
            else if (Size >= 2)
                value.extra = Data[1];
            else
                value.extra = 0;

            err = cbor_value_get_half_float_as_float(&value, &f_res);
            (void)err;
        } else if (selector == 1) {
            value.type = CborFloatType;
            if (Size >= 5) {
                uint32_t bits = read_u32(Data + 1);
                value.extra = (uint16_t)(bits & 0xffffu);
                value.remaining = bits;
            } else {
                value.extra = 0;
                value.remaining = 0;
            }

            if (cbor_value_is_float(&value)) {
                err = cbor_value_get_float(&value, &f_res);
                (void)err;
            }
        } else if (selector == 2) {
            value.type = CborDoubleType;
            if (Size >= 9) {
                uint64_t bits = read_u64(Data + 1);
                value.extra = (uint16_t)(bits & 0xffffu);
                value.remaining = (uint32_t)(bits & 0xffffffffu);
            } else {
                value.extra = 0;
                value.remaining = 0;
            }

            if (cbor_value_is_double(&value)) {
                err = cbor_value_get_double(&value, &d_res);
                (void)err;
            }
        } else {
            value.type = CborBooleanType;
            value.extra = (Size >= 2) ? (uint16_t)(Data[1] & 1u) : 0;
            err = cbor_value_get_boolean(&value, &b_res);
            (void)err;
        }
    }

    if (Size >= 2) {
        CborValue v2;
        memset(&v2, 0, sizeof(v2));
        v2.parser = &parser;
        v2.source.ptr = Data;
        v2.flags = 0;
        v2.remaining = (uint32_t)Size;

        switch (Data[Size - 1] % 3) {
            case 0:
                v2.type = CborHalfFloatType;
                v2.extra = (Size >= 2) ? read_u16(Data + (Size >= 3 ? Size - 2 : 0)) : 0;
                err = cbor_value_get_half_float_as_float(&v2, &f_res);
                (void)err;
                break;
            case 1:
                v2.type = CborFloatType;
                if (Size >= 4) {
                    uint32_t bits = read_u32(Data + Size - 4);
                    v2.extra = (uint16_t)(bits & 0xffffu);
                    v2.remaining = bits;
                }
                if (cbor_value_is_float(&v2)) {
                    err = cbor_value_get_float(&v2, &f_res);
                    (void)err;
                }
                break;
            case 2:
                v2.type = CborDoubleType;
                if (Size >= 8) {
                    uint64_t bits = read_u64(Data + Size - 8);
                    v2.extra = (uint16_t)(bits & 0xffffu);
                    v2.remaining = (uint32_t)(bits & 0xffffffffu);
                }
                if (cbor_value_is_double(&v2)) {
                    err = cbor_value_get_double(&v2, &d_res);
                    (void)err;
                }
                break;
        }
    }

    return 0;
}