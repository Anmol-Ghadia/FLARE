#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static int is_fixed_type_safe(const CborValue *it) {
    switch ((int)it->type) {
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

static int is_probably_valid_value(const CborValue *it) {
    if (!it || !it->parser)
        return 0;
    if (it->type == CborInvalidType)
        return 0;
    return 1;
}

static void maybe_validate_and_reparse(CborValue *it) {
    if (!is_probably_valid_value(it))
        return;
    (void)cbor_value_validate_basic(it);
    if (!is_probably_valid_value(it))
        return;
    (void)cbor_value_reparse(it);
}

static void exercise_string_iteration(const CborValue *src) {
    CborValue chunk;
    CborError err;
    int i;

    if (!src || !is_probably_valid_value(src))
        return;
    if (!cbor_value_is_text_string(src) && !cbor_value_is_byte_string(src))
        return;

    chunk = *src;
    err = cbor_value_begin_string_iteration(&chunk);
    if (err != CborNoError || !is_probably_valid_value(&chunk))
        return;

    for (i = 0; i < 32; ++i) {
        if (!is_probably_valid_value(&chunk))
            break;

        if (cbor_value_string_iteration_at_end(&chunk))
            break;

        /* Only call chunk-size API while actually iterating chunks. */
        if (chunk.flags & CborIteratorFlag_IteratingStringChunks) {
            size_t len = 0;
            (void)cbor_value_get_string_chunk_size(&chunk, &len);
        }

        err = cbor_value_advance(&chunk);
        if (err != CborNoError)
            break;
    }

    if (!is_probably_valid_value(&chunk))
        return;

    if (cbor_value_string_iteration_at_end(&chunk)) {
        (void)cbor_value_finish_string_iteration(&chunk);
    } else {
        (void)_cbor_value_finish_string_iteration(&chunk);
    }
}

static void exercise_value(CborValue *it, int depth) {
    CborValue rec;
    CborValue tmp;
    CborError err;
    int i;
    int rec_at_end = 0;

    if (depth > 4 || !is_probably_valid_value(it))
        return;

    maybe_validate_and_reparse(it);

    if (!is_probably_valid_value(it) || cbor_value_at_end(it))
        return;

    if (cbor_value_is_container(it)) {
        err = cbor_value_enter_container(it, &rec);
        if (err != CborNoError || !is_probably_valid_value(&rec))
            return;

        for (i = 0; i < 16; ++i) {
            if (!is_probably_valid_value(&rec))
                break;
            if (cbor_value_at_end(&rec)) {
                rec_at_end = 1;
                break;
            }

            exercise_value(&rec, depth + 1);

            if (!is_probably_valid_value(&rec))
                break;
            if (cbor_value_at_end(&rec)) {
                rec_at_end = 1;
                break;
            }

            err = cbor_value_advance(&rec);
            if (err != CborNoError)
                break;

            if (!is_probably_valid_value(&rec))
                break;
            if (cbor_value_at_end(&rec)) {
                rec_at_end = 1;
                break;
            }
        }

        if (rec_at_end)
            (void)cbor_value_leave_container(it, &rec);
        return;
    }

    if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it))
        exercise_string_iteration(it);

    if (!is_probably_valid_value(it) || cbor_value_at_end(it))
        return;

    tmp = *it;
    if (!is_probably_valid_value(&tmp))
        return;

    if (is_fixed_type_safe(&tmp)) {
        (void)cbor_value_advance_fixed(&tmp);
    } else {
        (void)cbor_value_advance(&tmp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    CborParser parser;
    CborValue it;
    CborValue cur;
    CborError err;
    int i;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError || !is_probably_valid_value(&it))
        return 0;

    maybe_validate_and_reparse(&it);

    if (!is_probably_valid_value(&it))
        return 0;

    cur = it;
    for (i = 0; i < 32; ++i) {
        if (!is_probably_valid_value(&cur) || cbor_value_at_end(&cur))
            break;

        exercise_value(&cur, 0);

        if (!is_probably_valid_value(&cur) || cbor_value_at_end(&cur))
            break;

        if (is_fixed_type_safe(&cur)) {
            err = cbor_value_advance_fixed(&cur);
        } else {
            err = cbor_value_advance(&cur);
        }
        if (err != CborNoError)
            break;

        if (!is_probably_valid_value(&cur) || cbor_value_at_end(&cur))
            break;

        maybe_validate_and_reparse(&cur);
    }

    return 0;
}