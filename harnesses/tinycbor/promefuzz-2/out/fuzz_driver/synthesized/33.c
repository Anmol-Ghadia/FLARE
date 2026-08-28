// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_raw_integer at cbor.h:418:27 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_get_int64_checked at cborparser.c:803:11 in cbor.h
// cbor_value_is_unsigned_integer at cbor.h:413:22 in cbor.h
// cbor_value_get_uint64 at cbor.h:425:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static void exercise_integer_accessors(const CborValue *v) {
    int64_t s64 = 0;
    uint64_t u64 = 0;
    uint64_t raw = 0;

    if (!v)
        return;

    if (cbor_value_is_integer(v)) {
        (void)cbor_value_get_raw_integer(v, &raw);
        (void)cbor_value_get_int64(v, &s64);
        (void)cbor_value_get_int64_checked(v, &s64);

        if (cbor_value_is_unsigned_integer(v))
            (void)cbor_value_get_uint64(v, &u64);
    }
}

static void walk_value(CborValue *it, int depth) {
    int steps = 0;

    if (!it || depth > 3)
        return;

    while (!cbor_value_at_end(it) && steps < 128) {
        exercise_integer_accessors(it);

        if (cbor_value_is_container(it)) {
            CborValue rec;
            CborError err = cbor_value_enter_container(it, &rec);
            if (err == CborNoError) {
                walk_value(&rec, depth + 1);
            }
        }

        if (cbor_value_advance(it) != CborNoError)
            break;
        ++steps;
    }
}

int LLVMFuzzerTestOneInput_33(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    walk_value(&it, 0);
    return 0;
}