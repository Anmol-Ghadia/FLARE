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
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include "cbor.h"

static void exercise_value(const CborValue *it) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!it)
        return;
    if (it->type == CborInvalidType)
        return;
    if (!cbor_value_is_integer(it))
        return;

    (void)cbor_value_get_raw_integer(it, &raw);
    (void)cbor_value_get_int64(it, &s64);
    (void)cbor_value_get_int64_checked(it, &s64);

    if (cbor_value_is_unsigned_integer(it))
        (void)cbor_value_get_uint64(it, &u64);
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 32)
        return;
    if (it->type == CborInvalidType)
        return;

    exercise_value(it);

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err;
        int count = 0;

        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&rec) && count++ < 128) {
            if (rec.type == CborInvalidType)
                break;

            walk_value(&rec, depth + 1);

            if (rec.type == CborInvalidType || cbor_value_at_end(&rec))
                break;

            err = cbor_value_advance(&rec);
            if (err != CborNoError)
                break;
        }

        if (rec.type != CborInvalidType && cbor_value_at_end(&rec))
            (void)cbor_value_leave_container(it, &rec);
    }
}

int LLVMFuzzerTestOneInput_21(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (!Data)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (it.type != CborInvalidType)
        walk_value(&it, 0);

    {
        CborValue cur = it;
        int steps = 0;

        while (cur.type != CborInvalidType && !cbor_value_at_end(&cur) && steps++ < 128) {
            exercise_value(&cur);

            if (cur.type == CborInvalidType || cbor_value_at_end(&cur))
                break;

            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}