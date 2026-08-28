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

static void exercise_value(const CborValue *value) {
    if (!value)
        return;

    (void)cbor_value_is_integer(value);
    (void)cbor_value_is_unsigned_integer(value);
    (void)cbor_value_is_negative_integer(value);

    if (cbor_value_is_integer(value)) {
        uint64_t raw = 0;
        int64_t i64 = 0;
        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64(value, &i64);

        if (cbor_value_is_unsigned_integer(value)) {
            uint64_t u64 = 0;
            (void)cbor_value_get_uint64(value, &u64);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    exercise_value(&cur);

    for (int depth = 0; depth < 32; ++depth) {
        exercise_value(&cur);

        CborValue recursed;
        if (cbor_value_is_container(&cur)) {
            if (cbor_value_enter_container(&cur, &recursed) == CborNoError) {
                CborValue inner = recursed;
                for (int inner_steps = 0; inner_steps < 32 && !cbor_value_at_end(&inner); ++inner_steps) {
                    exercise_value(&inner);
                    if (cbor_value_advance_fixed(&inner) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&cur, &inner);
            }
        }

        if (cbor_value_advance_fixed(&cur) != CborNoError)
            break;
        if (cbor_value_at_end(&cur))
            break;
    }

    if (Size >= 1) {
        uint8_t buf[9];
        size_t n = Size < sizeof(buf) ? Size : sizeof(buf);
        memcpy(buf, Data, n);

        err = cbor_parser_init(buf, n, 0, &parser, &it);
        if (err == CborNoError) {
            CborValue tmp = it;
            exercise_value(&tmp);
            if (!cbor_value_at_end(&tmp))
                (void)cbor_value_advance_fixed(&tmp);
        }
    }

    return 0;
}