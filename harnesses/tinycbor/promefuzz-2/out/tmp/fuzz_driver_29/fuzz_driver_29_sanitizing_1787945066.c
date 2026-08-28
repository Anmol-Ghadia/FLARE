#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

static void fuzz_value(const CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        int64_t s64 = 0;
        uint64_t u64 = 0;
        uint64_t raw = 0;
        int i = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int_checked(it, &i);

        if (cbor_value_is_unsigned_integer(it)) {
            (void)cbor_value_get_uint64(it, &u64);
        }

        if (raw <= (uint64_t)INT_MAX) {
            (void)cbor_value_get_int_checked(it, &i);
        }
    }
}

static void walk_container(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    fuzz_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err = cbor_value_enter_container(it, &rec);
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 128) {
                walk_container(&rec, depth + 1);
                err = cbor_value_advance(&rec);
                if (err != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    walk_container(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;
        while (err == CborNoError && !cbor_value_at_end(&cur) && steps++ < 128) {
            fuzz_value(&cur);
            if (cbor_value_is_container(&cur)) {
                CborValue rec;
                if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                    int inner = 0;
                    while (!cbor_value_at_end(&rec) && inner++ < 64) {
                        fuzz_value(&rec);
                        if (cbor_value_is_container(&rec)) {
                            walk_container(&rec, 1);
                        }
                        err = cbor_value_advance(&rec);
                        if (err != CborNoError)
                            break;
                    }
                    (void)cbor_value_leave_container(&cur, &rec);
                }
            }
            err = cbor_value_advance(&cur);
        }
    }

    return 0;
}