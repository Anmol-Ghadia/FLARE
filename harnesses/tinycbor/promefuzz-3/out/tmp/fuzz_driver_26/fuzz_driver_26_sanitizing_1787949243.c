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

static void exercise_value(const CborValue *it) {
    float f = 0.0f;
    uint16_t half = 0;
    double d = 0.0;
    (void)d;

    bool is_half = cbor_value_is_half_float(it);
    bool is_float = cbor_value_is_float(it);
    bool is_double = cbor_value_is_double(it);

    if (is_half) {
        (void)cbor_value_get_half_float(it, &half);
        (void)cbor_value_get_half_float_as_float(it, &f);
    }

    if (is_float) {
        (void)cbor_value_get_float(it, &f);
    }

    if (is_double) {
        /* Type check only; no target getter requested for double. */
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
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    size_t steps = 0;

    while (!cbor_value_at_end(&cur) && steps++ < 256) {
        exercise_value(&cur);

        if (cbor_value_is_container(&cur)) {
            CborValue recursed;
            if (cbor_value_enter_container(&cur, &recursed) == CborNoError) {
                size_t inner_steps = 0;
                while (!cbor_value_at_end(&recursed) && inner_steps++ < 256) {
                    exercise_value(&recursed);

                    if (cbor_value_is_container(&recursed)) {
                        CborValue nested;
                        if (cbor_value_enter_container(&recursed, &nested) == CborNoError) {
                            size_t nested_steps = 0;
                            while (!cbor_value_at_end(&nested) && nested_steps++ < 64) {
                                exercise_value(&nested);
                                if (cbor_value_advance(&nested) != CborNoError)
                                    break;
                            }
                            (void)cbor_value_leave_container(&recursed, &nested);
                        }
                    }

                    if (cbor_value_advance(&recursed) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&cur, &recursed);
            }
        }

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}