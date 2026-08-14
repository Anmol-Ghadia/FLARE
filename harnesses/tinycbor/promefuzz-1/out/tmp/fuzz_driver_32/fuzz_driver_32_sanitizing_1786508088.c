#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void consume_value(const CborValue *value) {
    CborType type = cbor_value_get_type(value);

    if (cbor_value_is_integer(value)) {
        int64_t i64 = 0;
        (void)cbor_value_get_int64(value, &i64);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge) {
            (void)_cbor_value_decode_int64_internal(value);
        }
    }

    if (cbor_value_is_tag(value)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(value, &tag);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge) {
            (void)_cbor_value_decode_int64_internal(value);
        }
    }

    if (type == CborDoubleType && cbor_value_is_double(value)) {
        double d = 0.0;
        (void)cbor_value_get_double(value, &d);
        (void)_cbor_value_extract_int64_helper(value);
        if (value->flags & CborIteratorFlag_IntegerValueTooLarge) {
            (void)_cbor_value_decode_int64_internal(value);
        }
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
                if (cbor_value_advance(&recursed) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &recursed);
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
    if (err == CborNoError) {
        traverse(&it, 0);
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        traverse(&it, 0);
    }

    err = cbor_parser_init(Data, Size, CborValidateCanonicalFormat, &parser, &it);
    if (err == CborNoError) {
        traverse(&it, 0);
    }

    return 0;
}