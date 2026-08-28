#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_value(const CborValue *it) {
    float f = 0.0f;
    uint16_t half = 0;

    if (cbor_value_is_half_float(it)) {
        (void)cbor_value_get_half_float(it, &half);
        (void)cbor_value_get_half_float_as_float(it, &f);
    }

    if (cbor_value_is_float(it)) {
        (void)cbor_value_get_float(it, &f);
    }

    (void)cbor_value_is_double(it);
}

static void walk_value(CborValue *it, size_t depth, size_t budget) {
    size_t steps = 0;

    while (!cbor_value_at_end(it) && it->type != CborInvalidType && steps++ < budget) {
        exercise_value(it);

        if (depth > 0 && cbor_value_is_container(it)) {
            CborValue recursed;
            CborError err = cbor_value_enter_container(it, &recursed);
            if (err == CborNoError) {
                walk_value(&recursed, depth - 1, budget / 2 + 1);
                if (recursed.type == CborInvalidType) {
                    (void)cbor_value_leave_container(it, &recursed);
                }
            }
        }

        if (it->type == CborInvalidType || cbor_value_at_end(it))
            break;

        if (cbor_value_advance(it) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (it.type != CborInvalidType)
        walk_value(&it, 3, 256);

    return 0;
}