#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cbor.h"

static void consume_value(const CborValue *value) {
    CborType type = cbor_value_get_type(value);

    if (cbor_value_is_integer(value)) {
        int64_t i64 = 0;
        (void)cbor_value_get_int64(value, &i64);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(value);
    }

    if (cbor_value_is_tag(value)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(value, &tag);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(value);
    }

    if (type == CborDoubleType && cbor_value_is_double(value)) {
        double d = 0.0;
        (void)cbor_value_get_double(value, &d);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(value);
    }
}

static void traverse(CborValue *it, int depth) {
    if (depth > 32 || cbor_value_at_end(it))
        return;

    consume_value(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        if (cbor_value_enter_container(it, &recursed) == CborNoError) {
            while (!cbor_value_at_end(&recursed)) {
                traverse(&recursed, depth + 1);

                if (cbor_value_at_end(&recursed))
                    break;

                if (cbor_value_advance(&recursed) != CborNoError)
                    break;
            }

            if (cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
        return;
    }
}

static void run_one_mode(const uint8_t *Data, size_t Size, int flags) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, flags, &parser, &it);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&it)) {
        traverse(&it, 0);
        if (cbor_value_at_end(&it))
            break;
        if (cbor_value_advance(&it) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    run_one_mode(Data, Size, 0);
    run_one_mode(Data, Size, CborValidateBasic);

    return 0;
}