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
    case CborInvalidType:
        return 1;
    default:
        return 0;
    }
}

static void exercise_value(CborValue *it) {
    if (!it)
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_reparse(it);

    for (int steps = 0; steps < 64 && !cbor_value_at_end(it); steps++) {
        (void)cbor_value_validate_basic(it);
        (void)cbor_value_reparse(it);

        CborType type = cbor_value_get_type(it);

        if (type == CborTextStringType || type == CborByteStringType) {
            CborValue recursed = *it;
            CborError err = cbor_value_enter_container(it, &recursed);
            if (err == CborNoError) {
                for (int chunks = 0; chunks < 64; chunks++) {
                    size_t len = 0;
                    (void)cbor_value_get_string_chunk_size(&recursed, &len);
                    if (cbor_value_at_end(&recursed))
                        break;

                    (void)cbor_value_validate_basic(&recursed);
                    (void)cbor_value_reparse(&recursed);

                    err = cbor_value_advance(&recursed);
                    if (err != CborNoError)
                        break;
                }

                if (cbor_value_at_end(&recursed))
                    (void)cbor_value_finish_string_iteration(&recursed);
                else
                    (void)_cbor_value_finish_string_iteration(&recursed);

                *it = recursed;
                continue;
            }
        }

        if (type == CborArrayType || type == CborMapType) {
            CborValue recursed;
            CborError err = cbor_value_enter_container(it, &recursed);
            if (err == CborNoError) {
                for (int inner = 0; inner < 16 && !cbor_value_at_end(&recursed); inner++) {
                    (void)cbor_value_validate_basic(&recursed);
                    (void)cbor_value_reparse(&recursed);

                    CborType inner_type = cbor_value_get_type(&recursed);
                    if (is_fixed_like_type(inner_type))
                        err = cbor_value_advance_fixed(&recursed);
                    else
                        err = cbor_value_advance(&recursed);

                    if (err != CborNoError)
                        break;
                }
                (void)cbor_value_leave_container(it, &recursed);
                continue;
            }
        }

        CborError err;
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

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        exercise_value(&it);
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        (void)cbor_value_validate_basic(&it);
        exercise_value(&it);
    }

    err = cbor_parser_init(Data, Size, CborValidateCanonicalFormat, &parser, &it);
    if (err == CborNoError) {
        (void)cbor_value_validate_basic(&it);
        (void)cbor_value_reparse(&it);
        exercise_value(&it);
    }

    return 0;
}