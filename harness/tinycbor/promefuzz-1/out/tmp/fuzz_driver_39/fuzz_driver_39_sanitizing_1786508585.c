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

#include <cbor.h>

static void process_value(const CborValue *value) {
    uint8_t simple = 0;
    float f = 0.0f;
    double d = 0.0;

    if (cbor_value_is_simple_type(value)) {
        (void)cbor_value_get_simple_type(value, &simple);
    }

    if (cbor_value_is_float(value)) {
        (void)cbor_value_get_float(value, &f);
    }

    if (cbor_value_is_double(value)) {
        (void)cbor_value_get_double(value, &d);
    }

    if (cbor_value_get_type(value) == CborHalfFloatType) {
        (void)cbor_value_get_half_float_as_float(value, &f);
    }
}

static void traverse_value(CborValue *it, int depth) {
    if (depth > 32) {
        return;
    }

    process_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError) {
            return;
        }

        while (!cbor_value_at_end(&recursed)) {
            traverse_value(&recursed, depth + 1);
            err = cbor_value_advance(&recursed);
            if (err != CborNoError) {
                break;
            }
        }

        (void)cbor_value_leave_container(it, &recursed);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    traverse_value(&it, 0);

    {
        CborValue copy = it;
        for (int i = 0; i < 8 && err == CborNoError && !cbor_value_at_end(&copy); ++i) {
            process_value(&copy);

            if (cbor_value_is_container(&copy)) {
                CborValue nested;
                err = cbor_value_enter_container(&copy, &nested);
                if (err == CborNoError) {
                    if (!cbor_value_at_end(&nested)) {
                        process_value(&nested);
                    }
                    (void)cbor_value_leave_container(&copy, &nested);
                }
            }

            err = cbor_value_advance(&copy);
        }
    }

    return 0;
}