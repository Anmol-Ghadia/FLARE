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

static void fuzz_one_value(const CborValue *it) {
    if (!it)
        return;

    bool is_int = cbor_value_is_integer(it);
    bool is_uint = cbor_value_is_unsigned_integer(it);
    bool is_nint = cbor_value_is_negative_integer(it);

    (void)is_uint;
    (void)is_nint;

    if (is_int) {
        uint64_t raw = 0;
        int64_t si = 0;
        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &si);

        if (cbor_value_is_unsigned_integer(it)) {
            uint64_t ui = 0;
            (void)cbor_value_get_uint64(it, &ui);
        }

        if (cbor_value_is_negative_integer(it)) {
            uint64_t raw2 = 0;
            int64_t si2 = 0;
            (void)cbor_value_get_raw_integer(it, &raw2);
            (void)cbor_value_get_int64(it, &si2);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    fuzz_one_value(&it);

    if (cbor_value_is_array(&it) || cbor_value_is_map(&it)) {
        CborValue rec;
        if (cbor_value_enter_container(&it, &rec) == CborNoError) {
            while (!cbor_value_at_end(&rec)) {
                fuzz_one_value(&rec);

                if (cbor_value_is_array(&rec) || cbor_value_is_map(&rec)) {
                    CborValue nested;
                    if (cbor_value_enter_container(&rec, &nested) == CborNoError) {
                        while (!cbor_value_at_end(&nested)) {
                            fuzz_one_value(&nested);
                            if (cbor_value_advance(&nested) != CborNoError)
                                break;
                        }
                        (void)cbor_value_leave_container(&rec, &nested);
                    }
                }

                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(&it, &rec);
        }
    }

    {
        CborValue walk = it;
        fuzz_one_value(&walk);
        (void)cbor_value_advance(&walk);
        fuzz_one_value(&walk);
    }

    return 0;
}