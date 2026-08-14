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

static void consume_value(CborValue *it);

static void handle_current_value(CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        (void)cbor_value_get_int64(it, &s);
        (void)_cbor_value_extract_int64_helper(it);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge) {
            (void)_cbor_value_decode_int64_internal(it);
        }
    } else if (type == CborTagType || cbor_value_is_tag(it)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(it, &tag);
    } else if (type == CborDoubleType || cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge) {
            (void)_cbor_value_decode_int64_internal(it);
        }
    }
}

static void consume_container(CborValue *it) {
    CborValue rec;
    CborError err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError)
        return;

    int limit = 0;
    while (!cbor_value_at_end(&rec) && limit++ < 64) {
        consume_value(&rec);
        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &rec);
}

static void consume_value(CborValue *it) {
    handle_current_value(it);

    CborType type = cbor_value_get_type(it);
    if (type == CborArrayType || type == CborMapType) {
        consume_container(it);
    } else if (type == CborTagType) {
        CborValue rec = *it;
        CborError err = cbor_value_advance_fixed(&rec);
        if (err == CborNoError && !cbor_value_at_end(&rec)) {
            handle_current_value(&rec);
            CborType inner = cbor_value_get_type(&rec);
            if (inner == CborArrayType || inner == CborMapType)
                consume_container(&rec);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    int top_limit = 0;
    while (!cbor_value_at_end(&it) && top_limit++ < 64) {
        consume_value(&it);
        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}