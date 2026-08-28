#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        CborValue cur = it;
        int steps = 0;

        while (!cbor_value_at_end(&cur) && steps < 128) {
            if (cbor_value_is_integer(&cur)) {
                int64_t s64 = 0;
                uint64_t u64 = 0;
                uint64_t raw = 0;

                (void)cbor_value_get_raw_integer(&cur, &raw);
                (void)cbor_value_get_int64(&cur, &s64);
                (void)cbor_value_get_int64_checked(&cur, &s64);

                if (cbor_value_is_unsigned_integer(&cur))
                    (void)cbor_value_get_uint64(&cur, &u64);
            }

            if (cbor_value_is_container(&cur)) {
                CborValue rec;
                if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                    int inner_steps = 0;
                    while (!cbor_value_at_end(&rec) && inner_steps < 32) {
                        if (cbor_value_is_integer(&rec)) {
                            int64_t s64 = 0;
                            uint64_t u64 = 0;
                            uint64_t raw = 0;

                            (void)cbor_value_get_raw_integer(&rec, &raw);
                            (void)cbor_value_get_int64(&rec, &s64);
                            (void)cbor_value_get_int64_checked(&rec, &s64);

                            if (cbor_value_is_unsigned_integer(&rec))
                                (void)cbor_value_get_uint64(&rec, &u64);
                        }

                        if (cbor_value_is_container(&rec)) {
                            CborValue nested;
                            if (cbor_value_enter_container(&rec, &nested) == CborNoError) {
                                int nested_steps = 0;
                                while (!cbor_value_at_end(&nested) && nested_steps < 8) {
                                    if (cbor_value_is_integer(&nested)) {
                                        int64_t s64 = 0;
                                        uint64_t u64 = 0;
                                        uint64_t raw = 0;

                                        (void)cbor_value_get_raw_integer(&nested, &raw);
                                        (void)cbor_value_get_int64(&nested, &s64);
                                        (void)cbor_value_get_int64_checked(&nested, &s64);

                                        if (cbor_value_is_unsigned_integer(&nested))
                                            (void)cbor_value_get_uint64(&nested, &u64);
                                    }

                                    if (cbor_value_advance(&nested) != CborNoError)
                                        break;
                                    ++nested_steps;
                                }
                                (void)cbor_value_leave_container(&rec, &nested);
                            }
                        }

                        if (cbor_value_advance(&rec) != CborNoError)
                            break;
                        ++inner_steps;
                    }
                    (void)cbor_value_leave_container(&cur, &rec);
                }
            }

            if (cbor_value_advance(&cur) != CborNoError)
                break;
            ++steps;
        }
    }

    return 0;
}