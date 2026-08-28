// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_uint64 at cbor.h:425:27 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_is_negative_integer at cbor.h:415:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cbor.h"

static void fuzz_one_value(const CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_integer(it)) {
        uint64_t raw = 0;
        int64_t si = 0;

        (void)cbor_value_get_raw_integer(it, &raw);
        (void)cbor_value_get_int64(it, &si);

        if (cbor_value_is_unsigned_integer(it)) {
            uint64_t ui = 0;
            (void)cbor_value_get_uint64(it, &ui);
        }

        if (cbor_value_is_negative_integer(it)) {
            uint64_t raw2 = 0;
            int64_t si2 = 0;
            (void)cbor_value_get_raw_integer(it, &raw2);
            (void)cbor_value_get_int64(it, &si2);
        }
    } else {
        (void)cbor_value_is_unsigned_integer(it);
        (void)cbor_value_is_negative_integer(it);
    }
}

static void fuzz_container(CborValue *container, int depth) {
    CborValue rec;
    CborError err;

    if (!container || depth > 3)
        return;

    if (!cbor_value_is_array(container) && !cbor_value_is_map(container))
        return;

    err = cbor_value_enter_container(container, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec)) {
        fuzz_one_value(&rec);

        if ((cbor_value_is_array(&rec) || cbor_value_is_map(&rec)) && depth < 3) {
            CborValue nested = rec;
            fuzz_container(&nested, depth + 1);
        }

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            return;
    }

    (void)cbor_value_leave_container(container, &rec);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    fuzz_one_value(&it);

    if (cbor_value_is_array(&it) || cbor_value_is_map(&it)) {
        CborValue top = it;
        fuzz_container(&top, 0);
    }

    return 0;
}