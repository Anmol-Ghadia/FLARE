#include <stdint.h>
#include <stddef.h>
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

        if (cbor_value_is_unsigned_integer(it))
            (void)cbor_value_get_uint64(it, &u64);
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
            /* Do not call cbor_value_leave_container() here because rec may not
               be positioned at CborInvalidType if advancing failed or traversal
               stopped early, which triggers internal assertions. */
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;

    if (Data == NULL)
        return 0;

    if (cbor_parser_init(Data, Size, 0, &parser, &it) == CborNoError) {
        CborValue cur = it;
        int limit = 0;
        while (!cbor_value_at_end(&cur) && limit++ < 64) {
            fuzz_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    if (cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it) == CborNoError) {
        CborValue cur = it;
        int limit = 0;
        while (!cbor_value_at_end(&cur) && limit++ < 64) {
            fuzz_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    return 0;
}