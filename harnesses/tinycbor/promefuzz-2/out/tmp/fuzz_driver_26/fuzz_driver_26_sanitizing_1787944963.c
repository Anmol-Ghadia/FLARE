#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cbor.h"

static void consume_integer_value(const CborValue *it) {
    int ires = 0;
    int64_t i64res = 0;
    uint64_t u64res = 0;
    uint64_t rawres = 0;

    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        (void)cbor_value_get_int(it, &ires);
        (void)cbor_value_get_int64(it, &i64res);
        (void)cbor_value_get_raw_integer(it, &rawres);

        if (cbor_value_is_unsigned_integer(it))
            (void)cbor_value_get_uint64(it, &u64res);
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;

    consume_integer_value(it);

    if (cbor_value_is_array(it) || cbor_value_is_map(it)) {
        CborValue rec;
        CborError err;
        int count = 0;

        memset(&rec, 0, sizeof(rec));
        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&rec) && count++ < 64) {
            walk_value(&rec, depth + 1);
            if (cbor_value_advance(&rec) != CborNoError)
                break;
        }

        /* Do not call cbor_value_leave_container() unless the recursive iterator
           is positioned at end/invalid as required by tinycbor assertions. */
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    walk_value(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;

        while (!cbor_value_at_end(&cur) && steps++ < 64) {
            consume_integer_value(&cur);
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    return 0;
}