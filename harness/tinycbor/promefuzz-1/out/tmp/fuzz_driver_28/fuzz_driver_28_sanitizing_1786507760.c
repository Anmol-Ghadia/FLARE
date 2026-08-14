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

static void consume_container(CborValue *it) {
    CborError err;
    CborValue recursed;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed)) {
        consume_value(&recursed);
        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            break;
    }

    (void)cbor_value_leave_container(it, &recursed);
}

static void consume_value(CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        uint64_t raw = _cbor_value_extract_int64_helper(it);
        (void)raw;
        (void)cbor_value_get_int64(it, &s);

        if ((it->flags & CborIteratorFlag_IntegerValueTooLarge) != 0) {
            uint64_t big = _cbor_value_decode_int64_internal(it);
            (void)big;
        }
    } else if (cbor_value_is_tag(it)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(it, &tag);
    } else if (cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
        if ((it->flags & CborIteratorFlag_IntegerValueTooLarge) != 0) {
            uint64_t bits = _cbor_value_decode_int64_internal(it);
            (void)bits;
        }
    } else if (type == CborArrayType || type == CborMapType) {
        consume_container(it);
    } else {
        (void)type;
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

    while (!cbor_value_at_end(&it)) {
        consume_value(&it);
        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    if (Size > 0) {
        CborParser parser2;
        CborValue it2;
        size_t split = Data[0] % (Size + 1);

        err = cbor_parser_init(Data, split, 0, &parser2, &it2);
        if (err == CborNoError) {
            while (!cbor_value_at_end(&it2)) {
                consume_value(&it2);
                err = cbor_value_advance(&it2);
                if (err != CborNoError)
                    break;
            }
        }
    }

    return 0;
}