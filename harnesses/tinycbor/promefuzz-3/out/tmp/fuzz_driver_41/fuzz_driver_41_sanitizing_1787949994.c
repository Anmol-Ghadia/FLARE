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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    for (int steps = 0; steps < 64; ++steps) {
        bool is_int = cbor_value_is_integer(&cur);

        if (is_int) {
            uint64_t raw = 0;
            int64_t val = 0;
            int64_t checked = 0;

            (void)cbor_value_is_unsigned_integer(&cur);
            (void)cbor_value_is_negative_integer(&cur);

            (void)cbor_value_get_raw_integer(&cur, &raw);
            (void)cbor_value_get_int64(&cur, &val);
            (void)cbor_value_get_int64_checked(&cur, &checked);

            if (cbor_value_is_unsigned_integer(&cur)) {
                (void)cbor_value_get_raw_integer(&cur, &raw);
                (void)cbor_value_get_int64_checked(&cur, &checked);
            }

            if (cbor_value_is_negative_integer(&cur)) {
                (void)cbor_value_get_int64(&cur, &val);
                (void)cbor_value_get_int64_checked(&cur, &checked);
            }
        }

        CborValue recursed;
        CborType type = cbor_value_get_type(&cur);
        if ((type == CborArrayType || type == CborMapType) &&
            cbor_value_is_container(&cur) &&
            cbor_value_enter_container(&cur, &recursed) == CborNoError) {
            CborValue inner = recursed;
            for (int inner_steps = 0; inner_steps < 16 && !cbor_value_at_end(&inner); ++inner_steps) {
                if (cbor_value_is_integer(&inner)) {
                    uint64_t raw = 0;
                    int64_t val = 0;
                    int64_t checked = 0;
                    (void)cbor_value_is_unsigned_integer(&inner);
                    (void)cbor_value_is_negative_integer(&inner);
                    (void)cbor_value_get_raw_integer(&inner, &raw);
                    (void)cbor_value_get_int64(&inner, &val);
                    (void)cbor_value_get_int64_checked(&inner, &checked);
                }

                if (cbor_value_advance(&inner) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(&cur, &inner);
        }

        if (cbor_value_at_end(&cur))
            break;
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}