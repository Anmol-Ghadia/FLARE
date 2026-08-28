#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static uint16_t read_u16(const uint8_t *p) {
    uint16_t v = 0;
    memcpy(&v, p, sizeof(v));
    return v;
}

static CborValue make_value(const CborParser *parser, const uint8_t *ptr, size_t size) {
    CborValue v;
    memset(&v, 0, sizeof(v));
    v.parser = parser;
    v.source.ptr = ptr;
    v.remaining = (uint32_t)size;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    memset(&parser, 0, sizeof(parser));
    parser.source.end = Data + Size;
    parser.flags = 0;

    float f_res = 0.0f;
    bool b_res = false;
    CborError err;

    if (Size >= 1) {
        CborValue value = make_value(&parser, Data, Size);
        switch (Data[0] % 3) {
            case 0:
                value.type = CborHalfFloatType;
                value.extra = (Size >= 3) ? read_u16(Data + 1) : 0;
                err = cbor_value_get_half_float_as_float(&value, &f_res);
                (void)err;
                break;
            case 1:
                value.type = CborBooleanType;
                value.extra = (Size >= 2) ? (uint16_t)(Data[1] & 1u) : 0;
                err = cbor_value_get_boolean(&value, &b_res);
                (void)err;
                break;
            case 2:
                value.type = CborFloatType;
                if (cbor_value_is_float(&value)) {
                    err = cbor_value_get_float(&value, &f_res);
                    (void)err;
                }
                break;
        }
    }

    if (Size >= 2) {
        CborValue v2 = make_value(&parser, Data + 1, Size - 1);
        switch (Data[Size - 1] % 3) {
            case 0:
                v2.type = CborHalfFloatType;
                v2.extra = (Size >= 3) ? read_u16(Data + Size - 2) : 0;
                err = cbor_value_get_half_float_as_float(&v2, &f_res);
                (void)err;
                break;
            case 1:
                v2.type = CborBooleanType;
                v2.extra = (uint16_t)(Data[Size - 1] & 1u);
                err = cbor_value_get_boolean(&v2, &b_res);
                (void)err;
                break;
            case 2:
                v2.type = CborFloatType;
                if (cbor_value_is_float(&v2)) {
                    err = cbor_value_get_float(&v2, &f_res);
                    (void)err;
                }
                break;
        }
    }

    return 0;
}