#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "cbor.h"

static void process_value(const CborValue *value) {
    CborType type = cbor_value_get_type(value);

    if (type == CborFloatType && cbor_value_is_float(value)) {
        float f = 0.0f;
        (void)cbor_value_get_float(value, &f);
    } else if (type == CborDoubleType && cbor_value_is_double(value)) {
        double d = 0.0;
        (void)cbor_value_get_double(value, &d);
    } else if (type == CborHalfFloatType) {
        float hf = 0.0f;
        (void)cbor_value_get_half_float_as_float(value, &hf);
    }
}

static void traverse_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    process_value(it);

    {
        CborType type = cbor_value_get_type(it);
        if (type == CborArrayType || type == CborMapType) {
            CborValue recursed;
            CborError err = cbor_value_enter_container(it, &recursed);
            if (err != CborNoError)
                return;

            while (!cbor_value_at_end(&recursed)) {
                CborType inner_type = cbor_value_get_type(&recursed);
                if (inner_type == CborInvalidType)
                    break;

                traverse_value(&recursed, depth + 1);

                if (cbor_value_at_end(&recursed))
                    break;

                inner_type = cbor_value_get_type(&recursed);
                if (inner_type == CborInvalidType)
                    break;

                err = cbor_value_advance_fixed(&recursed);
                if (err != CborNoError)
                    break;
            }

            if (cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        if (cbor_value_get_type(&it) != CborInvalidType)
            traverse_value(&it, 0);
    }

    if (Size >= 3) {
        uint8_t buf[9];
        size_t n = 0;

        switch (Data[0] % 3) {
        case 0:
            buf[0] = 0xf9;
            buf[1] = Data[1];
            buf[2] = Data[2];
            n = 3;
            break;
        case 1:
            if (Size >= 5) {
                buf[0] = 0xfa;
                memcpy(buf + 1, Data + 1, 4);
                n = 5;
            }
            break;
        default:
            if (Size >= 9) {
                buf[0] = 0xfb;
                memcpy(buf + 1, Data + 1, 8);
                n = 9;
            }
            break;
        }

        if (n != 0) {
            err = cbor_parser_init(buf, n, 0, &parser, &it);
            if (err == CborNoError) {
                if (cbor_value_get_type(&it) != CborInvalidType)
                    process_value(&it);
            }
        }
    }

    return 0;
}