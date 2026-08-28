#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <cbor.h>

static void consume_value(const CborValue *it) {
    if (cbor_value_is_float(it)) {
        float f = 0.0f;
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_half_float(it)) {
        uint16_t half = 0;
        float f = 0.0f;
        (void)cbor_value_get_half_float(it, &half);
        (void)cbor_value_get_half_float_as_float(it, &f);
    }

    (void)cbor_value_is_double(it);
}

static void traverse(CborValue *it, int depth) {
    if (depth > 32 || cbor_value_at_end(it)) {
        return;
    }

    consume_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        if (cbor_value_enter_container(it, &recursed) != CborNoError) {
            return;
        }

        while (!cbor_value_at_end(&recursed)) {
            traverse(&recursed, depth + 1);
            if (cbor_value_advance(&recursed) != CborNoError) {
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
    if (fp != NULL) {
        (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError && !cbor_value_at_end(&it)) {
        traverse(&it, 0);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        int steps = 0;
        while (!cbor_value_at_end(&it) && steps++ < 64) {
            consume_value(&it);
            if (cbor_value_advance(&it) != CborNoError) {
                break;
            }
        }
    }

    return 0;
}