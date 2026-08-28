#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cbor.h"

static uint64_t read_u64_le(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    if (n > 8)
        n = 8;
    for (i = 0; i < n; ++i)
        v |= ((uint64_t)p[i]) << (8 * i);
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    size_t offset = 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        CborValue cur = it;
        int steps = 0;

        while (!cbor_value_at_end(&cur) && steps < 64) {
            if (cbor_value_is_integer(&cur)) {
                int64_t s64 = 0;
                uint64_t u64 = 0;
                uint64_t raw = 0;

                (void)cbor_value_get_raw_integer(&cur, &raw);
                (void)cbor_value_get_int64(&cur, &s64);
                (void)cbor_value_get_int64_checked(&cur, &s64);

                if (cbor_value_is_unsigned_integer(&cur))
                    (void)cbor_value_get_uint64(&cur, &u64);

                if ((steps & 1) == 0) {
                    (void)cbor_value_get_int64(&cur, &s64);
                    if (cbor_value_is_unsigned_integer(&cur))
                        (void)cbor_value_get_uint64(&cur, &u64);
                }
            }

            if (cbor_value_is_container(&cur)) {
                CborValue rec;
                if (cbor_value_enter_container(&cur, &rec) == CborNoError) {
                    int inner_steps = 0;
                    while (!cbor_value_at_end(&rec) && inner_steps < 16) {
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

    if (Size >= 2) {
        CborValue manual;
        CborParser fake_parser;
        uint64_t raw = 0;
        int64_t s64 = 0;
        uint64_t u64 = 0;
        uint8_t flags_sel = Data[0];
        uint8_t type_sel = Data[1];

        memset(&manual, 0, sizeof(manual));
        memset(&fake_parser, 0, sizeof(fake_parser));

        manual.parser = &fake_parser;
        manual.source.ptr = Data + offset;
        manual.remaining = 1;
        manual.extra = (uint16_t)(read_u64_le(Data + (Size > 2 ? 2 : 0), Size > 2 ? Size - 2 : 0) & 0xffffu);
        manual.type = (type_sel & 1) ? CborIntegerType : CborNegativeIntegerType;
        manual.flags = 0;

        if (flags_sel & 1)
            manual.type = CborIntegerType;
        else
            manual.type = CborNegativeIntegerType;

        if (cbor_value_is_integer(&manual)) {
            (void)cbor_value_get_raw_integer(&manual, &raw);
            (void)cbor_value_get_int64(&manual, &s64);
            (void)cbor_value_get_int64_checked(&manual, &s64);
            if (cbor_value_is_unsigned_integer(&manual))
                (void)cbor_value_get_uint64(&manual, &u64);
        }
    }

    return 0;
}