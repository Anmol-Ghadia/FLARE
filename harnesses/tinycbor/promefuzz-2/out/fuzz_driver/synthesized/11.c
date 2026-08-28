// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_uint64 at cbor.h:425:27 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static void fuzz_value(const CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        int64_t s64 = 0;
        uint64_t u64 = 0;
        uint64_t raw = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &s64);
        (void)cbor_value_get_int64_checked(it, &s64);

        if (cbor_value_is_unsigned_integer(it))
            (void)cbor_value_get_uint64(it, &u64);
    }

    if (cbor_value_is_container(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            size_t limit = 0;
            while (!cbor_value_at_end(&rec) && limit++ < 256) {
                fuzz_value(&rec);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            /* Do not call cbor_value_leave_container() here because rec may not
             * be positioned at the required invalid end state if we stopped
             * early or hit an error, which can trigger internal assertions. */
        }
    }
}

int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
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
    if (err == CborNoError)
        fuzz_value(&it);

    {
        CborParser parser2;
        CborValue it2;
        err = cbor_parser_init(Data, Size, CborValidateBasic, &parser2, &it2);
        if (err == CborNoError)
            fuzz_value(&it2);
    }

    return 0;
}