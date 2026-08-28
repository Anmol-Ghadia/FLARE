#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cbor.h"

static void call_target_sequence(const CborValue *v) {
    if (cbor_value_is_tag(v)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(v, &tag);
    }

    if (cbor_value_is_simple_type(v)) {
        uint8_t simple = 0;
        (void)cbor_value_get_simple_type(v, &simple);
    }

    if (cbor_value_is_boolean(v)) {
        bool b = false;
        (void)cbor_value_get_boolean(v, &b);
    }

    if (cbor_value_is_float(v)) {
        float f = 0.0f;
        (void)cbor_value_get_float(v, &f);
    }

    if (cbor_value_is_double(v)) {
        double d = 0.0;
        (void)cbor_value_get_double(v, &d);
    }

    if (cbor_value_is_half_float(v)) {
        uint16_t hf = 0;
        (void)cbor_value_get_half_float(v, &hf);
    }
}

static bool walk_value(CborValue *it, int depth) {
    if (depth > 8)
        return false;

    while (!cbor_value_at_end(it)) {
        CborValue cur = *it;
        call_target_sequence(&cur);

        if (cbor_value_is_container(&cur)) {
            CborValue rec;
            if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                bool fully_consumed = walk_value(&rec, depth + 1);
                if (fully_consumed && cbor_value_at_end(&rec)) {
                    (void)cbor_value_leave_container(&cur, &rec);
                } else {
                    return false;
                }
            }
        }

        if (cbor_value_advance(it) != CborNoError)
            return false;
    }

    return true;
}

static void exercise_buffer(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    (void)walk_value(&it, 0);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    exercise_buffer(Data, Size);

    if (Size > 1) {
        size_t split = Size / 2;
        exercise_buffer(Data, split);
        exercise_buffer(Data + split, Size - split);
    }

    if (Size > 0) {
        uint8_t local[4096];
        size_t n = Size < sizeof(local) ? Size : sizeof(local);
        memcpy(local, Data, n);

        local[0] ^= 0xff;
        exercise_buffer(local, n);
        local[0] ^= 0xff;

        if (n > 1) {
            uint8_t tmp = local[0];
            local[0] = local[n - 1];
            local[n - 1] = tmp;
            exercise_buffer(local, n);
        }
    }

    return 0;
}