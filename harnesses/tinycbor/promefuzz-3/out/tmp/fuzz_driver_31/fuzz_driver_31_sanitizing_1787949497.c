#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static int is_fixed_like_type(CborType type) {
    switch ((int)type) {
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

static void exercise_chunked_string(CborValue *it) {
    CborValue chunk;
    CborError err;

    if (!it || cbor_value_at_end(it))
        return;

    if (cbor_value_get_type(it) != CborTextStringType &&
        cbor_value_get_type(it) != CborByteStringType)
        return;

    if (cbor_value_is_length_known(it))
        return;

    err = cbor_value_begin_string_iteration(it, &chunk);
    if (err != CborNoError)
        return;

    for (int i = 0; i < 64; i++) {
        size_t len = 0;

        (void)cbor_value_reparse(&chunk);
        err = cbor_value_get_string_chunk_size(&chunk, &len);
        if (err != CborNoError)
            break;

        (void)cbor_value_validate_basic(&chunk);
        (void)cbor_value_reparse(&chunk);

        err = cbor_value_advance(&chunk);
        if (err != CborNoError)
            break;

        if (cbor_value_string_iteration_at_end(&chunk)) {
            (void)cbor_value_finish_string_iteration(&chunk);
            *it = chunk;
            return;
        }
    }

    if (cbor_value_string_iteration_at_end(&chunk)) {
        (void)cbor_value_finish_string_iteration(&chunk);
        *it = chunk;
    } else {
        (void)_cbor_value_finish_string_iteration(&chunk);
        *it = chunk;
    }
}

static void exercise_value(CborValue *it) {
    if (!it)
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_reparse(it);

    for (int steps = 0; steps < 64 && !cbor_value_at_end(it); steps++) {
        CborType type;
        CborError err;

        (void)cbor_value_validate_basic(it);
        (void)cbor_value_reparse(it);

        type = cbor_value_get_type(it);

        if ((type == CborTextStringType || type == CborByteStringType) &&
            !cbor_value_is_length_known(it)) {
            exercise_chunked_string(it);
            continue;
        }

        if (type == CborArrayType || type == CborMapType) {
            CborValue recursed;
            err = cbor_value_enter_container(it, &recursed);
            if (err == CborNoError) {
                int completed = 1;

                for (int inner = 0; inner < 16; inner++) {
                    CborType inner_type;

                    if (cbor_value_at_end(&recursed))
                        break;

                    (void)cbor_value_validate_basic(&recursed);
                    (void)cbor_value_reparse(&recursed);

                    inner_type = cbor_value_get_type(&recursed);

                    if ((inner_type == CborTextStringType || inner_type == CborByteStringType) &&
                        !cbor_value_is_length_known(&recursed)) {
                        exercise_chunked_string(&recursed);
                        continue;
                    }

                    if (inner_type == CborArrayType || inner_type == CborMapType) {
                        err = cbor_value_advance(&recursed);
                    } else if (is_fixed_like_type(inner_type)) {
                        err = cbor_value_advance_fixed(&recursed);
                    } else {
                        err = cbor_value_advance(&recursed);
                    }

                    if (err != CborNoError) {
                        completed = 0;
                        break;
                    }
                }

                if (completed && cbor_value_at_end(&recursed))
                    (void)cbor_value_leave_container(it, &recursed);
                else
                    (void)cbor_value_advance(it);

                continue;
            }
        }

        if (is_fixed_like_type(type))
            err = cbor_value_advance_fixed(it);
        else
            err = cbor_value_advance(it);

        if (err != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        CborParser parser;
        CborValue it;
        CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
        if (err == CborNoError)
            exercise_value(&it);
    }

    {
        CborParser parser;
        CborValue it;
        CborError err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
        if (err == CborNoError) {
            (void)cbor_value_validate_basic(&it);
            exercise_value(&it);
        }
    }

    return 0;
}