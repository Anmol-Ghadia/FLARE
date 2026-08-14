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

static void process_value(CborValue *it);

static void process_container(CborValue *it) {
    CborValue recursed;
    CborError err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        process_value(&recursed);
        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void process_value(CborValue *it) {
    CborType type = cbor_value_get_type(it);
    (void)type;

    if (cbor_value_is_float(it)) {
        float f = 0.0f;
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
    }

    if (type == CborHalfFloatType) {
        float hf = 0.0f;
        (void)cbor_value_get_half_float_as_float(it, &hf);
    }

    if (type == CborArrayType || type == CborMapType) {
        process_container(it);
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
        process_value(&it);

        CborValue walk = it;
        while (true) {
            process_value(&walk);
            err = cbor_value_advance(&walk);
            if (err != CborNoError)
                break;
        }
    }

    if (Size >= 3) {
        uint8_t buf[9];
        CborParser p2;
        CborValue v2;

        buf[0] = 0xf9;
        buf[1] = Data[0];
        buf[2] = Data[1];
        err = cbor_parser_init(buf, 3, 0, &p2, &v2);
        if (err == CborNoError) {
            if (cbor_value_get_type(&v2) == CborHalfFloatType) {
                float hf = 0.0f;
                (void)cbor_value_get_half_float_as_float(&v2, &hf);
            }
        }

        buf[0] = 0xfa;
        if (Size >= 4) {
            memcpy(buf + 1, Data, 4);
            err = cbor_parser_init(buf, 5, 0, &p2, &v2);
            if (err == CborNoError) {
                if (cbor_value_is_float(&v2)) {
                    float f = 0.0f;
                    (void)cbor_value_get_float(&v2, &f);
                }
                (void)cbor_value_get_type(&v2);
            }
        }

        buf[0] = 0xfb;
        if (Size >= 8) {
            memcpy(buf + 1, Data, 8);
            err = cbor_parser_init(buf, 9, 0, &p2, &v2);
            if (err == CborNoError) {
                if (cbor_value_is_double(&v2)) {
                    double d = 0.0;
                    (void)cbor_value_get_double(&v2, &d);
                }
                (void)cbor_value_get_type(&v2);
            }
        }
    }

    return 0;
}