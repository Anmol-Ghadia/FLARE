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

static int is_fixed_type_safe(CborType type) {
    switch (type) {
    case CborIntegerType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborSimpleType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
        return 1;
    default:
        return 0;
    }
}

static void exercise_string_iteration(CborValue *it) {
    CborValue copy = *it;
    (void)cbor_value_reparse(&copy);
    (void)cbor_value_validate_basic(&copy);

    if (cbor_value_is_text_string(&copy) || cbor_value_is_byte_string(&copy)) {
        CborValue recursed = copy;
        CborError err = cbor_value_enter_container(&copy, &recursed);
        if (err == CborNoError) {
            for (int i = 0; i < 32; ++i) {
                size_t len = 0;
                (void)cbor_value_get_string_chunk_size(&recursed, &len);
                (void)cbor_value_validate_basic(&recursed);
                (void)cbor_value_reparse(&recursed);

                if (cbor_value_at_end(&recursed))
                    break;

                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }

            if (cbor_value_at_end(&recursed)) {
                (void)cbor_value_finish_string_iteration(&recursed);
            } else {
                (void)_cbor_value_finish_string_iteration(&recursed);
            }
        }
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || depth > 8)
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_reparse(it);

    if (cbor_value_at_end(it))
        return;

    exercise_string_iteration(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            for (int i = 0; i < 32 && !cbor_value_at_end(&recursed); ++i) {
                walk_value(&recursed, depth + 1);
                if (cbor_value_at_end(&recursed))
                    break;

                if (is_fixed_type_safe((CborType)recursed.type))
                    err = cbor_value_advance_fixed(&recursed);
                else
                    err = cbor_value_advance(&recursed);

                if (err != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &recursed);
        }
        return;
    }

    if (is_fixed_type_safe((CborType)it->type))
        (void)cbor_value_advance_fixed(it);
    else
        (void)cbor_value_advance(it);
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

    (void)cbor_value_validate_basic(&it);
    (void)cbor_value_reparse(&it);

    CborValue copy1 = it;
    CborValue copy2 = it;
    CborValue copy3 = it;

    walk_value(&it, 0);
    walk_value(&copy1, 1);

    if (!cbor_value_at_end(&copy2)) {
        if (is_fixed_type_safe((CborType)copy2.type))
            (void)cbor_value_advance_fixed(&copy2);
        else
            (void)cbor_value_advance(&copy2);
        (void)cbor_value_reparse(&copy2);
        (void)cbor_value_validate_basic(&copy2);
        exercise_string_iteration(&copy2);
    }

    if (!cbor_value_at_end(&copy3)) {
        exercise_string_iteration(&copy3);
        (void)cbor_value_reparse(&copy3);
        (void)cbor_value_validate_basic(&copy3);
    }

    return 0;
}