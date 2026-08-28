#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cbor.h"

static void process_value(const CborValue *value) {
    float f = 0.0f;
    uint16_t half_raw = 0;
    bool is_half = cbor_value_is_half_float(value);
    bool is_float = cbor_value_is_float(value);
    bool is_double = cbor_value_is_double(value);

    (void)is_double;

    if (is_half) {
        (void)cbor_value_get_half_float(value, &half_raw);
        (void)cbor_value_get_half_float_as_float(value, &f);
    }

    if (is_float) {
        (void)cbor_value_get_float(value, &f);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    CborParser parser;
    CborValue it;
    CborError err;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    process_value(&it);

    {
        CborValue cur = it;
        size_t steps = 0;

        while (!cbor_value_at_end(&cur) && steps++ < 64) {
            process_value(&cur);

            if (cbor_value_is_container(&cur)) {
                CborValue recursed;
                if (cbor_value_enter_container(&cur, &recursed) == CborNoError) {
                    size_t inner_steps = 0;
                    while (!cbor_value_at_end(&recursed) && inner_steps++ < 64) {
                        process_value(&recursed);
                        if (cbor_value_advance(&recursed) != CborNoError)
                            break;
                    }
                    (void)cbor_value_leave_container(&cur, &recursed);
                    continue;
                }
            }

            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    if (Size >= 1) {
        uint8_t buf[9];
        size_t n = Size < sizeof(buf) ? Size : sizeof(buf);
        memcpy(buf, Data, n);

        if (n >= 3) {
            buf[0] = 0xf9;
            {
                CborParser p2;
                CborValue v2;
                if (cbor_parser_init(buf, n, 0, &p2, &v2) == CborNoError)
                    process_value(&v2);
            }
        }

        if (n >= 5) {
            buf[0] = 0xfa;
            {
                CborParser p3;
                CborValue v3;
                if (cbor_parser_init(buf, n, 0, &p3, &v3) == CborNoError)
                    process_value(&v3);
            }
        }

        if (n >= 9) {
            buf[0] = 0xfb;
            {
                CborParser p4;
                CborValue v4;
                if (cbor_parser_init(buf, n, 0, &p4, &v4) == CborNoError)
                    process_value(&v4);
            }
        }
    }

    return 0;
}