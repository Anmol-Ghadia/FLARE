// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_value_is_tag at cbor.h:457:22 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_is_array at cbor.h:565:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_calculate_string_length at cborparser.c:954:11 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_copy_text_string at cbor.h:493:27 in cbor.h
// cbor_value_copy_byte_string at cbor.h:499:27 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static int is_definitely_fixed_type(uint8_t type) {
    switch ((CborType)type) {
    case CborIntegerType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborSimpleType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
    case CborTagType:
        return 1;
    default:
        return 0;
    }
}

static int is_valid_iterator(const CborValue *it) {
    return it && it->type != CborInvalidType;
}

static void consume_string_if_possible(CborValue *it) {
    size_t len = 0;

    if (!is_valid_iterator(it))
        return;

    if (cbor_value_is_byte_string(it) || cbor_value_is_text_string(it)) {
        (void)cbor_value_calculate_string_length(it, &len);
        if (len > 0 && len < 4096) {
            char buf[4096];
            size_t n = len;
            if (cbor_value_is_text_string(it)) {
                (void)cbor_value_copy_text_string(it, buf, &n, NULL);
            } else {
                (void)cbor_value_copy_byte_string(it, (uint8_t *)buf, &n, NULL);
            }
        }
    }
}

static void exercise_iterator(const CborValue *orig, int depth);

static void walk_container(CborValue *it, int depth) {
    CborValue recursed;
    CborError err;
    int steps = 0;

    if (!is_valid_iterator(it) || depth > 8)
        return;

    err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed) && is_valid_iterator(&recursed) && steps++ < 64) {
        exercise_iterator(&recursed, depth + 1);

        if (cbor_value_at_end(&recursed) || !is_valid_iterator(&recursed))
            break;

        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            return;
    }

    if (cbor_value_at_end(&recursed) && recursed.type == CborInvalidType)
        (void)cbor_value_leave_container(it, &recursed);
}

static void exercise_map_find(const CborValue *it) {
    static const char *keys[] = {
        "", "a", "key", "id", "type", "name", "value", "dummy"
    };
    size_t i;

    if (!is_valid_iterator(it) || !cbor_value_is_map(it))
        return;

    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        CborValue element;
        CborError err = cbor_value_map_find_value(it, keys[i], &element);
        if (err != CborNoError)
            continue;

        if (!is_valid_iterator(&element))
            continue;

        (void)cbor_value_validate_basic(&element);

        if (is_definitely_fixed_type(element.type)) {
            CborValue tmp = element;
            (void)cbor_value_advance_fixed(&tmp);
        }
    }
}

static void exercise_iterator(const CborValue *orig, int depth) {
    CborValue it;
    CborValue tmp;

    if (!is_valid_iterator(orig))
        return;

    it = *orig;

    if (!is_valid_iterator(&it))
        return;

    (void)cbor_value_validate_basic(&it);

    tmp = it;
    if (is_valid_iterator(&tmp) && cbor_value_skip_tag(&tmp) == CborNoError && is_valid_iterator(&tmp))
        (void)cbor_value_validate_basic(&tmp);

    if (is_valid_iterator(&it) && is_definitely_fixed_type(it.type)) {
        tmp = it;
        (void)cbor_value_advance_fixed(&tmp);
    }

    if (is_valid_iterator(&it) && cbor_value_is_tag(&it)) {
        tmp = it;
        if (cbor_value_skip_tag(&tmp) == CborNoError && is_valid_iterator(&tmp)) {
            (void)cbor_value_validate_basic(&tmp);
            if (is_definitely_fixed_type(tmp.type)) {
                CborValue tmp2 = tmp;
                (void)cbor_value_advance_fixed(&tmp2);
            }
        }
    }

    consume_string_if_possible(&it);
    exercise_map_find(&it);

    if (!is_valid_iterator(&it))
        return;

    if (cbor_value_is_array(&it) || cbor_value_is_map(&it))
        walk_container(&it, depth);
    else
        (void)cbor_value_advance(&it);
}

int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    int top_steps = 0;
    FILE *fp = fopen("./dummy_file", "wb");

    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    while (!cbor_value_at_end(&it) && is_valid_iterator(&it) && top_steps++ < 64) {
        exercise_iterator(&it, 0);

        if (cbor_value_at_end(&it) || !is_valid_iterator(&it))
            break;

        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}