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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    while (!cbor_value_at_end(&it)) {
        CborValue current = it;

        if (cbor_value_is_tag(&current)) {
            CborTag tag = 0;
            (void)cbor_value_get_tag(&current, &tag);
        }

        if (cbor_value_is_simple_type(&current)) {
            uint8_t simple = 0;
            (void)cbor_value_get_simple_type(&current, &simple);
        }

        if (cbor_value_is_boolean(&current)) {
            bool b = false;
            (void)cbor_value_get_boolean(&current, &b);
        }

        if (cbor_value_is_float(&current)) {
            float f = 0.0f;
            (void)cbor_value_get_float(&current, &f);
        }

        if (cbor_value_is_double(&current)) {
            double d = 0.0;
            (void)cbor_value_get_double(&current, &d);
        }

        if (cbor_value_is_half_float(&current)) {
            uint16_t hf = 0;
            (void)cbor_value_get_half_float(&current, &hf);
        }

        if (cbor_value_is_container(&current)) {
            CborValue recursed;
            if (cbor_value_enter_container(&current, &recursed) == CborNoError) {
                CborValue inner = recursed;
                size_t limit = 32;
                while (!cbor_value_at_end(&inner) && limit--) {
                    if (cbor_value_is_tag(&inner)) {
                        CborTag tag = 0;
                        (void)cbor_value_get_tag(&inner, &tag);
                    }

                    if (cbor_value_is_simple_type(&inner)) {
                        uint8_t simple = 0;
                        (void)cbor_value_get_simple_type(&inner, &simple);
                    }

                    if (cbor_value_is_boolean(&inner)) {
                        bool b = false;
                        (void)cbor_value_get_boolean(&inner, &b);
                    }

                    if (cbor_value_is_float(&inner)) {
                        float f = 0.0f;
                        (void)cbor_value_get_float(&inner, &f);
                    }

                    if (cbor_value_is_double(&inner)) {
                        double d = 0.0;
                        (void)cbor_value_get_double(&inner, &d);
                    }

                    if (cbor_value_is_half_float(&inner)) {
                        uint16_t hf = 0;
                        (void)cbor_value_get_half_float(&inner, &hf);
                    }

                    if (cbor_value_advance(&inner) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&current, &inner);
            }
        }

        if (cbor_value_advance(&it) != CborNoError)
            break;
    }

    return 0;
}