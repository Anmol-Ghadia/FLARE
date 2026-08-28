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

        if (cbor_value_is_unsigned_integer(it)) {
            (void)cbor_value_get_uint64(it, &u64);
        }
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
            (void)cbor_value_leave_container(it, &rec);
        }
    }
}

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

    fuzz_value(&it);

    {
        CborValue cur = it;
        size_t steps = 0;
        while (!cbor_value_at_end(&cur) && steps++ < 256) {
            fuzz_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    {
        CborParser parser2;
        CborValue it2;
        err = cbor_parser_init(Data, Size, CborValidateBasic, &parser2, &it2);
        if (err == CborNoError)
            fuzz_value(&it2);
    }

    return 0;
}