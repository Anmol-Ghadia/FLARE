#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cbor.h"

static void process_value(const CborValue *value) {
    float f = 0.0f;
    uint16_t half_raw = 0;

    if (cbor_value_is_half_float(value)) {
        (void)cbor_value_get_half_float(value, &half_raw);
        (void)cbor_value_get_half_float_as_float(value, &f);
    }

    if (cbor_value_is_float(value)) {
        (void)cbor_value_get_float(value, &f);
    }

    (void)cbor_value_is_double(value);
}

static void walk_value(CborValue *it, size_t depth) {
    size_t steps = 0;

    if (depth > 8 || it == NULL)
        return;

    while (!cbor_value_at_end(it) && steps++ < 64) {
        process_value(it);

        if (cbor_value_is_container(it)) {
            CborValue recursed;
            CborError err = cbor_value_enter_container(it, &recursed);
            if (err != CborNoError)
                break;

            walk_value(&recursed, depth + 1);

            if (cbor_value_leave_container(it, &recursed) != CborNoError)
                break;

            continue;
        }

        if (cbor_value_advance_fixed(it) != CborNoError)
            break;
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
    if (err == CborNoError) {
        CborValue root = it;
        walk_value(&root, 0);
    }

    if (Size >= 3) {
        uint8_t buf[9] = {0};
        size_t n = Size < sizeof(buf) ? Size : sizeof(buf);
        memcpy(buf, Data, n);

        buf[0] = 0xf9;
        {
            CborParser p2;
            CborValue v2;
            if (cbor_parser_init(buf, n, 0, &p2, &v2) == CborNoError)
                process_value(&v2);
        }

        if (n >= 5) {
            memcpy(buf, Data, n);
            buf[0] = 0xfa;
            {
                CborParser p3;
                CborValue v3;
                if (cbor_parser_init(buf, n, 0, &p3, &v3) == CborNoError)
                    process_value(&v3);
            }
        }

        if (n >= 9) {
            memcpy(buf, Data, n);
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