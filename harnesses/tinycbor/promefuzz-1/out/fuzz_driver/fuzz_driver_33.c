// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_integer at cbor.h:411:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// _cbor_value_extract_int64_helper at cbor.h:373:26 in cbor.h
// _cbor_value_decode_int64_internal at cborparser.c:316:10 in cbor.h
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_get_tag at cbor.h:459:27 in cbor.h
// cbor_value_is_double at cbor.h:624:22 in cbor.h
// cbor_value_get_double at cbor.h:626:27 in cbor.h
// _cbor_value_decode_int64_internal at cborparser.c:316:10 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cbor.h"

static void exercise_accessors(const CborValue *it) {
    CborType type = cbor_value_get_type(it);

    if (cbor_value_is_integer(it)) {
        int64_t s = 0;
        (void)cbor_value_get_int64(it, &s);
        (void)_cbor_value_extract_int64_helper(it);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
    } else if (type == CborTagType && cbor_value_is_tag(it)) {
        CborTag tag = 0;
        (void)cbor_value_get_tag(it, &tag);
    } else if (type == CborDoubleType && cbor_value_is_double(it)) {
        double d = 0.0;
        (void)cbor_value_get_double(it, &d);
        if (it->flags & CborIteratorFlag_IntegerValueTooLarge)
            (void)_cbor_value_decode_int64_internal(it);
    }
}

static void walk_value(CborValue *it, int depth);

static void walk_container(CborValue *it, int depth) {
    CborValue rec;
    CborError err;
    int limit = 0;

    err = cbor_value_enter_container(it, &rec);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&rec) && limit++ < 64) {
        CborType t = cbor_value_get_type(&rec);
        if (t == CborInvalidType)
            break;

        walk_value(&rec, depth + 1);

        err = cbor_value_advance(&rec);
        if (err != CborNoError)
            break;

        if (cbor_value_get_type(&rec) == CborInvalidType)
            break;
    }
}

static void walk_tag_payload(CborValue *it, int depth) {
    CborValue tagged = *it;
    CborError err = cbor_value_advance_fixed(&tagged);
    if (err != CborNoError || cbor_value_at_end(&tagged))
        return;
    if (cbor_value_get_type(&tagged) == CborInvalidType)
        return;

    walk_value(&tagged, depth + 1);
}

static void walk_value(CborValue *it, int depth) {
    if (depth > 16)
        return;
    if (cbor_value_at_end(it))
        return;
    if (cbor_value_get_type(it) == CborInvalidType)
        return;

    exercise_accessors(it);

    switch (cbor_value_get_type(it)) {
    case CborArrayType:
    case CborMapType:
        walk_container(it, depth);
        break;
    case CborTagType:
        walk_tag_payload(it, depth);
        break;
    default:
        break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (cbor_value_at_end(&it))
        return 0;
    if (cbor_value_get_type(&it) == CborInvalidType)
        return 0;

    walk_value(&it, 0);
    return 0;
}