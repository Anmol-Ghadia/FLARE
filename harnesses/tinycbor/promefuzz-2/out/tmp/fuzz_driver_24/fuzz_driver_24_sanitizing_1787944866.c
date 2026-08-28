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

static void fuzz_one_item(const uint8_t *buf, size_t len) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&it)) {
        CborType type = cbor_value_get_type(&it);

        if (cbor_value_is_integer(&it)) {
            int64_t s64 = 0;
            uint64_t u64 = 0;
            uint64_t raw = 0;

            (void)cbor_value_get_raw_integer(&it, &raw);
            (void)cbor_value_get_int64(&it, &s64);
            (void)cbor_value_get_int64_checked(&it, &s64);

            if (cbor_value_is_unsigned_integer(&it)) {
                (void)cbor_value_get_uint64(&it, &u64);
            }
        }

        if (type == CborArrayType || type == CborMapType) {
            CborValue rec;
            err = cbor_value_enter_container(&it, &rec);
            if (err == CborNoError) {
                size_t limit = 32;
                while (!cbor_value_at_end(&rec) && limit--) {
                    if (cbor_value_is_integer(&rec)) {
                        int64_t s64 = 0;
                        uint64_t u64 = 0;
                        uint64_t raw = 0;

                        (void)cbor_value_get_raw_integer(&rec, &raw);
                        (void)cbor_value_get_int64(&rec, &s64);
                        (void)cbor_value_get_int64_checked(&rec, &s64);

                        if (cbor_value_is_unsigned_integer(&rec)) {
                            (void)cbor_value_get_uint64(&rec, &u64);
                        }
                    }

                    if (cbor_value_advance(&rec) != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(&it, &rec);
            } else {
                break;
            }
        }

        if (cbor_value_advance(&it) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    fuzz_one_item(Data, Size);

    if (Size > 0) {
        uint8_t stackbuf[256];
        size_t n = Size < sizeof(stackbuf) ? Size : sizeof(stackbuf);
        memcpy(stackbuf, Data, n);
        fuzz_one_item(stackbuf, n);
    }

    if (Size >= 2) {
        size_t split = Data[0] % Size;
        fuzz_one_item(Data, split);
        fuzz_one_item(Data + split, Size - split);
    }

    return 0;
}