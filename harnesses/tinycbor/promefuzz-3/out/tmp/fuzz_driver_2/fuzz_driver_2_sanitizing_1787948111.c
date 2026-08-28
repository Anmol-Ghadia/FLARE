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

static void exercise_value(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&it)) {
        CborValue cur = it;

        if (cbor_value_is_tag(&cur)) {
            CborTag tag = 0;
            (void)cbor_value_get_tag(&cur, &tag);
        }

        if (cbor_value_is_simple_type(&cur)) {
            uint8_t simple = 0;
            (void)cbor_value_get_simple_type(&cur, &simple);
        }

        if (cbor_value_is_boolean(&cur)) {
            bool b = false;
            (void)cbor_value_get_boolean(&cur, &b);
        }

        if (cbor_value_is_float(&cur)) {
            float f = 0.0f;
            (void)cbor_value_get_float(&cur, &f);
        }

        if (cbor_value_is_double(&cur)) {
            double d = 0.0;
            (void)cbor_value_get_double(&cur, &d);
        }

        if (cbor_value_is_half_float(&cur)) {
            uint16_t hf = 0;
            (void)cbor_value_get_half_float(&cur, &hf);
        }

        if (cbor_value_is_container(&cur)) {
            CborValue rec;
            if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                while (!cbor_value_at_end(&rec)) {
                    CborValue nested = rec;

                    if (cbor_value_is_tag(&nested)) {
                        CborTag tag = 0;
                        (void)cbor_value_get_tag(&nested, &tag);
                    }

                    if (cbor_value_is_simple_type(&nested)) {
                        uint8_t simple = 0;
                        (void)cbor_value_get_simple_type(&nested, &simple);
                    }

                    if (cbor_value_is_boolean(&nested)) {
                        bool b = false;
                        (void)cbor_value_get_boolean(&nested, &b);
                    }

                    if (cbor_value_is_float(&nested)) {
                        float f = 0.0f;
                        (void)cbor_value_get_float(&nested, &f);
                    }

                    if (cbor_value_is_double(&nested)) {
                        double d = 0.0;
                        (void)cbor_value_get_double(&nested, &d);
                    }

                    if (cbor_value_is_half_float(&nested)) {
                        uint16_t hf = 0;
                        (void)cbor_value_get_half_float(&nested, &hf);
                    }

                    if (cbor_value_advance(&rec) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&cur, &rec);
            }
        }

        if (cbor_value_advance(&it) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    exercise_value(Data, Size);

    if (Size > 1) {
        size_t split = Size / 2;
        exercise_value(Data, split);
        exercise_value(Data + split, Size - split);
    }

    if (Size > 0) {
        uint8_t local[4096];
        size_t n = Size < sizeof(local) ? Size : sizeof(local);
        memcpy(local, Data, n);

        if (n > 0) {
            local[0] ^= 0xff;
            exercise_value(local, n);
            local[0] ^= 0xff;
        }

        if (n > 1) {
            uint8_t tmp = local[0];
            local[0] = local[n - 1];
            local[n - 1] = tmp;
            exercise_value(local, n);
        }
    }

    return 0;
}