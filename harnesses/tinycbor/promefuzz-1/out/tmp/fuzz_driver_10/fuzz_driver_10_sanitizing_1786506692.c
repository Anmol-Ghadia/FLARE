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

static int is_fixed_type(CborType type) {
    switch (type) {
    case CborIntegerType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborSimpleType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
    case CborByteStringType:
    case CborTextStringType:
    case CborTagType:
        return 1;
    default:
        return 0;
    }
}

static void consume_string_if_possible(CborValue *it) {
    size_t len = 0;
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
    if (depth > 8)
        return;

    CborValue recursed;
    CborError err = cbor_value_enter_container(it, &recursed);
    if (err != CborNoError)
        return;

    int steps = 0;
    while (!cbor_value_at_end(&recursed) && steps++ < 64) {
        exercise_iterator(&recursed, depth + 1);

        if (cbor_value_at_end(&recursed))
            break;

        err = cbor_value_advance(&recursed);
        if (err != CborNoError)
            return;
    }

    if (cbor_value_at_end(&recursed)) {
        (void)cbor_value_leave_container(it, &recursed);
    }
}

static void exercise_map_find(const CborValue *it) {
    if (!cbor_value_is_map(it))
        return;

    static const char *keys[] = {
        "",
        "a",
        "key",
        "id",
        "type",
        "name",
        "value",
        "dummy"
    };
    size_t i;
    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        CborValue element;
        if (cbor_value_map_find_value(it, keys[i], &element) == CborNoError) {
            (void)cbor_value_validate_basic(&element);
            if (is_fixed_type((CborType)element.type)) {
                CborValue tmp = element;
                (void)cbor_value_advance_fixed(&tmp);
            }
        }
    }
}

static void exercise_iterator(const CborValue *orig, int depth) {
    CborValue it = *orig;
    (void)cbor_value_validate_basic(&it);

    CborValue skipped = it;
    (void)cbor_value_skip_tag(&skipped);
    (void)cbor_value_validate_basic(&skipped);

    if (is_fixed_type((CborType)it.type)) {
        CborValue tmp = it;
        (void)cbor_value_advance_fixed(&tmp);
    }

    if (cbor_value_is_tag(&it)) {
        CborValue tmp = it;
        if (cbor_value_skip_tag(&tmp) == CborNoError) {
            (void)cbor_value_validate_basic(&tmp);
            if (is_fixed_type((CborType)tmp.type)) {
                CborValue tmp2 = tmp;
                (void)cbor_value_advance_fixed(&tmp2);
            }
        }
    }

    consume_string_if_possible(&it);
    exercise_map_find(&it);

    if (cbor_value_is_array(&it) || cbor_value_is_map(&it)) {
        walk_container(&it, depth);
    } else {
        CborValue tmp = it;
        (void)cbor_value_advance(&tmp);
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
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    int top_steps = 0;
    while (!cbor_value_at_end(&it) && top_steps++ < 64) {
        exercise_iterator(&it, 0);

        if (cbor_value_at_end(&it))
            break;

        err = cbor_value_advance(&it);
        if (err != CborNoError)
            break;
    }

    return 0;
}