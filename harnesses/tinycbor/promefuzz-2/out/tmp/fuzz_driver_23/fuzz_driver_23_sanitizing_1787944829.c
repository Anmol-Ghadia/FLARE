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

static void fuzz_one(const CborParser *parser, const uint8_t *ptr, size_t size, uint8_t selector) {
    CborValue value = make_value(parser, ptr, size);
    CborError err;
    float f_res = 0.0f;
    double d_res = 0.0;
    bool b_res = false;

    switch (selector % 4) {
        case 0:
            if (size < 2)
                return;
            value.type = CborHalfFloatType;
            value.extra = read_u16(ptr);
            err = cbor_value_get_half_float_as_float(&value, &f_res);
            (void)err;
            break;

        case 1:
            if (size < 1)
                return;
            value.type = CborBooleanType;
            value.extra = (uint16_t)(ptr[0] & 1u);
            err = cbor_value_get_boolean(&value, &b_res);
            (void)err;
            break;

        case 2:
            if (size < 5)
                return;
            value.type = CborFloatType;
            if (cbor_value_is_float(&value)) {
                err = cbor_value_get_float(&value, &f_res);
                (void)err;
            }
            break;

        case 3:
            if (size < 9)
                return;
            value.type = CborDoubleType;
            if (cbor_value_is_double(&value)) {
                err = cbor_value_get_double(&value, &d_res);
                (void)err;
            }
            break;
    }
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

    if (Size > 0)
        fuzz_one(&parser, Data, Size, Data[0]);

    if (Size > 1)
        fuzz_one(&parser, Data + 1, Size - 1, Data[Size - 1]);

    if (Size > 2)
        fuzz_one(&parser, Data + (Size / 2), Size - (Size / 2), Data[1]);

    return 0;
}