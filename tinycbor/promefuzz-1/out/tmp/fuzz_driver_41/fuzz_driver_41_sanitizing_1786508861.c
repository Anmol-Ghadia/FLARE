#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void process_value(CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_simple_type(it)) {
        uint8_t simple = 0;
        (void)cbor_value_get_simple_type(it, &simple);
    }

    if (cbor_value_is_float(it)) {
        float f = 0.0f;
        (void)cbor_value_get_float(it, &f);
    }

    if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
    }

    if (cbor_value_is_half_float(it)) {
        float hf = 0.0f;
        (void)cbor_value_get_half_float_as_float(it, &hf);
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 16)
        return;

    process_value(it);

    if (cbor_value_is_array(it) || cbor_value_is_map(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError)
            return;

        {
            int count = 0;
            while (!cbor_value_at_end(&recursed) && count < 32) {
                walk_value(&recursed, depth + 1);

                if (cbor_value_at_end(&recursed))
                    break;

                count++;
                /* Do not advance here: recursive processing may already have
                 * consumed nested containers and left recursed positioned at
                 * the next sibling. Advancing again can move past the end and
                 * trigger assertions. */
            }
        }

        if (cbor_value_at_end(&recursed))
            (void)cbor_value_leave_container(it, &recursed);
    } else if (cbor_value_is_tag(it)) {
        CborValue tagged = *it;
        CborError err = cbor_value_advance_fixed(&tagged);
        if (err == CborNoError)
            walk_value(&tagged, depth + 1);
    } else {
        (void)cbor_value_advance_fixed(it);
    }
}

static void fuzz_buffer(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    {
        CborValue root = it;
        walk_value(&root, 0);
    }

    {
        CborValue cursor = it;
        int i;
        for (i = 0; i < 8 && !cbor_value_at_end(&cursor); i++) {
            process_value(&cursor);

            if (cbor_value_is_array(&cursor) || cbor_value_is_map(&cursor)) {
                CborValue tmp = cursor;
                walk_value(&tmp, 0);
                break;
            } else if (cbor_value_is_tag(&cursor)) {
                err = cbor_value_advance_fixed(&cursor);
                if (err != CborNoError)
                    break;
            } else {
                err = cbor_value_advance_fixed(&cursor);
                if (err != CborNoError)
                    break;
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    fuzz_buffer(Data, Size);

    if (Size >= 3) {
        uint8_t buf[3];
        memcpy(buf, Data, 3);
        buf[0] = 0xf9;
        fuzz_buffer(buf, sizeof(buf));
    }

    if (Size >= 5) {
        uint8_t buf[5];
        memcpy(buf, Data, 5);
        buf[0] = 0xfa;
        fuzz_buffer(buf, sizeof(buf));
    }

    if (Size >= 9) {
        uint8_t buf[9];
        memcpy(buf, Data, 9);
        buf[0] = 0xfb;
        fuzz_buffer(buf, sizeof(buf));
    }

    if (Size >= 1) {
        uint8_t buf[2];
        buf[0] = 0xf8;
        buf[1] = Data[0];
        fuzz_buffer(buf, sizeof(buf));
    }

    return 0;
}