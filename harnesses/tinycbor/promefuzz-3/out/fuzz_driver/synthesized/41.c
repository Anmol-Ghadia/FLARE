// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_41(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue root;
    CborError err;

    if (Size > 0) {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &root);
    if (err != CborNoError)
        return 0;

    CborValue cur = root;
    for (int steps = 0; steps < 64; ++steps) {
        if (cbor_value_is_integer(&cur)) {
            uint64_t raw = 0;
            int64_t val = 0;
            int64_t checked = 0;

            (void)cbor_value_is_unsigned_integer(&cur);
            (void)cbor_value_is_negative_integer(&cur);
            (void)cbor_value_get_raw_integer(&cur, &raw);
            (void)cbor_value_get_int64(&cur, &val);
            (void)cbor_value_get_int64_checked(&cur, &checked);

            if (cbor_value_is_unsigned_integer(&cur)) {
                (void)cbor_value_get_raw_integer(&cur, &raw);
                (void)cbor_value_get_int64_checked(&cur, &checked);
            }

            if (cbor_value_is_negative_integer(&cur)) {
                (void)cbor_value_get_int64(&cur, &val);
                (void)cbor_value_get_int64_checked(&cur, &checked);
            }
        }

        if (cbor_value_is_container(&cur)) {
            CborValue recursed;
            if (cbor_value_enter_container(&cur, &recursed) == CborNoError) {
                CborValue inner = recursed;
                for (int inner_steps = 0; inner_steps < 16 && !cbor_value_at_end(&inner); ++inner_steps) {
                    if (cbor_value_is_integer(&inner)) {
                        uint64_t raw = 0;
                        int64_t val = 0;
                        int64_t checked = 0;

                        (void)cbor_value_is_unsigned_integer(&inner);
                        (void)cbor_value_is_negative_integer(&inner);
                        (void)cbor_value_get_raw_integer(&inner, &raw);
                        (void)cbor_value_get_int64(&inner, &val);
                        (void)cbor_value_get_int64_checked(&inner, &checked);
                    }

                    if (cbor_value_advance(&inner) != CborNoError)
                        break;
                }

                if (cbor_value_at_end(&inner))
                    (void)cbor_value_leave_container(&cur, &inner);
            }
        }

        if (cbor_value_at_end(&cur))
            break;
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}