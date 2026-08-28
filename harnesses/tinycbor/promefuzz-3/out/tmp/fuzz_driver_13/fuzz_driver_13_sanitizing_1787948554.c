#include <stdint.h>
#include <stddef.h>
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

static int is_valid_iterator_type(const CborValue *it) {
    return it && it->type != CborInvalidType;
}

static void maybe_validate(CborValue *it) {
    if (!is_valid_iterator_type(it) || cbor_value_at_end(it))
        return;
    (void)cbor_value_validate_basic(it);
}

static void maybe_reparse(CborValue *it) {
    if (!is_valid_iterator_type(it) || cbor_value_at_end(it))
        return;
    (void)cbor_value_reparse(it);
}

static void exercise_string_iteration(CborValue *it) {
    CborValue copy;
    CborError err;
    int i;

    if (!is_valid_iterator_type(it) || cbor_value_at_end(it))
        return;

    copy = *it;
    maybe_reparse(&copy);
    maybe_validate(&copy);

    if (!(cbor_value_is_text_string(&copy) || cbor_value_is_byte_string(&copy)))
        return;

    if (!cbor_value_is_length_known(&copy)) {
        CborValue chunks = copy;

        err = cbor_value_enter_container(&copy, &chunks);
        if (err != CborNoError || !is_valid_iterator_type(&chunks))
            return;

        for (i = 0; i < 32; ++i) {
            size_t len = 0;

            if (!is_valid_iterator_type(&chunks))
                break;

            if (cbor_value_at_end(&chunks))
                break;

            (void)cbor_value_get_string_chunk_size(&chunks, &len);
            maybe_validate(&chunks);
            maybe_reparse(&chunks);

            if (!is_valid_iterator_type(&chunks) || cbor_value_at_end(&chunks))
                break;

            err = cbor_value_advance(&chunks);
            if (err != CborNoError || !is_valid_iterator_type(&chunks))
                break;
        }

        if (is_valid_iterator_type(&chunks)) {
            if (cbor_value_at_end(&chunks))
                (void)cbor_value_finish_string_iteration(&chunks);
            else
                (void)_cbor_value_finish_string_iteration(&chunks);
        }
    }
}

static void walk_value(CborValue *it, int depth) {
    CborError err;

    if (!is_valid_iterator_type(it) || depth > 8 || cbor_value_at_end(it))
        return;

    maybe_validate(it);
    maybe_reparse(it);

    if (!is_valid_iterator_type(it) || cbor_value_at_end(it))
        return;

    exercise_string_iteration(it);

    if (!is_valid_iterator_type(it) || cbor_value_at_end(it))
        return;

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        int i;

        err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError || !is_valid_iterator_type(&recursed))
            return;

        for (i = 0; i < 32; ++i) {
            if (!is_valid_iterator_type(&recursed) || cbor_value_at_end(&recursed))
                break;

            walk_value(&recursed, depth + 1);

            if (!is_valid_iterator_type(&recursed) || cbor_value_at_end(&recursed))
                break;

            if (is_fixed_type_safe((CborType)recursed.type))
                err = cbor_value_advance_fixed(&recursed);
            else
                err = cbor_value_advance(&recursed);

            if (err != CborNoError || !is_valid_iterator_type(&recursed))
                break;
        }

        if (is_valid_iterator_type(it) && is_valid_iterator_type(&recursed))
            (void)cbor_value_leave_container(it, &recursed);
        return;
    }

    if (is_fixed_type_safe((CborType)it->type))
        (void)cbor_value_advance_fixed(it);
    else
        (void)cbor_value_advance(it);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it, copy1, copy2, copy3;
    CborError err;
    FILE *fp = fopen("./dummy_file", "wb");

    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError || !is_valid_iterator_type(&it))
        return 0;

    maybe_validate(&it);
    maybe_reparse(&it);

    if (!is_valid_iterator_type(&it))
        return 0;

    copy1 = it;
    copy2 = it;
    copy3 = it;

    walk_value(&it, 0);
    walk_value(&copy1, 1);

    if (is_valid_iterator_type(&copy2) && !cbor_value_at_end(&copy2)) {
        if (is_fixed_type_safe((CborType)copy2.type))
            err = cbor_value_advance_fixed(&copy2);
        else
            err = cbor_value_advance(&copy2);

        if (err == CborNoError && is_valid_iterator_type(&copy2) && !cbor_value_at_end(&copy2)) {
            maybe_reparse(&copy2);
            maybe_validate(&copy2);
            exercise_string_iteration(&copy2);
        }
    }

    if (is_valid_iterator_type(&copy3) && !cbor_value_at_end(&copy3)) {
        exercise_string_iteration(&copy3);
        if (is_valid_iterator_type(&copy3) && !cbor_value_at_end(&copy3)) {
            maybe_reparse(&copy3);
            maybe_validate(&copy3);
        }
    }

    return 0;
}