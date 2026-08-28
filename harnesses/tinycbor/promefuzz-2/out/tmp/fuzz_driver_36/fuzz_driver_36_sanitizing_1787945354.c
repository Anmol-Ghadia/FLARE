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

        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int64_checked(it, &s64);
        (void)cbor_value_get_raw_integer(it, &raw);

        if (cbor_value_is_unsigned_integer(it)) {
            (void)cbor_value_get_uint64(it, &u64);
        }
    }

    if (cbor_value_is_container(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int limit = 0;
            while (!cbor_value_at_end(&rec) && limit++ < 64) {
                fuzz_value(&rec);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;

    if (!Data)
        return 0;

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    {
        CborValue cur = it;
        int limit = 0;
        while (!cbor_value_at_end(&cur) && limit++ < 64) {
            fuzz_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    if (Size > 0) {
        CborParser parser2;
        CborValue it2;
        if (cbor_parser_init(Data, Size, CborValidateBasic, &parser2, &it2) == CborNoError) {
            CborValue cur2 = it2;
            int limit2 = 0;
            while (!cbor_value_at_end(&cur2) && limit2++ < 64) {
                fuzz_value(&cur2);
                if (cbor_value_advance(&cur2) != CborNoError)
                    break;
            }
        }
    }

    return 0;
}