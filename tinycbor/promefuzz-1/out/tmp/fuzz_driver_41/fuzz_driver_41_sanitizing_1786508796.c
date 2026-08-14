#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&recursed) && count < 32) {
                walk_value(&recursed, depth + 1);
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
                count++;
            }
            (void)cbor_value_leave_container(it, &recursed);
        }
    } else if (cbor_value_is_tag(it)) {
        CborValue tagged = *it;
        CborError err = cbor_value_advance_fixed(&tagged);
        if (err == CborNoError)
            walk_value(&tagged, depth + 1);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        walk_value(&it, 0);

        CborValue cursor = it;
        for (int i = 0; i < 8 && !cbor_value_at_end(&cursor); i++) {
            process_value(&cursor);
            err = cbor_value_advance(&cursor);
            if (err != CborNoError)
                break;
        }
    }

    if (Size >= 3) {
        uint8_t buf[3];
        memcpy(buf, Data, 3);
        buf[0] = 0xf9;
        err = cbor_parser_init(buf, sizeof(buf), 0, &parser, &it);
        if (err == CborNoError) {
            process_value(&it);
        }
    }

    if (Size >= 5) {
        uint8_t buf[5];
        memcpy(buf, Data, 5);
        buf[0] = 0xfa;
        err = cbor_parser_init(buf, sizeof(buf), 0, &parser, &it);
        if (err == CborNoError) {
            process_value(&it);
        }
    }

    if (Size >= 9) {
        uint8_t buf[9];
        memcpy(buf, Data, 9);
        buf[0] = 0xfb;
        err = cbor_parser_init(buf, sizeof(buf), 0, &parser, &it);
        if (err == CborNoError) {
            process_value(&it);
        }
    }

    if (Size >= 1) {
        uint8_t buf[2];
        buf[0] = 0xf8;
        buf[1] = Data[0];
        err = cbor_parser_init(buf, sizeof(buf), 0, &parser, &it);
        if (err == CborNoError) {
            process_value(&it);
        }
    }

    return 0;
}