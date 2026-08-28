#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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

static void exercise_value(CborValue *it) {
    CborValue cur = *it;
    int steps = 0;

    while (!cbor_value_at_end(&cur) && steps++ < 64) {
        (void)cbor_value_validate_basic(&cur);
        (void)cbor_value_reparse(&cur);

        if (!cbor_value_is_valid(&cur))
            break;

        if (cbor_value_is_text_string(&cur) || cbor_value_is_byte_string(&cur)) {
            if (!cbor_value_is_length_known(&cur)) {
                CborValue strit = cur;
                CborError err = cbor_value_begin_string_iteration(&strit);
                if (err == CborNoError) {
                    int chunk_steps = 0;
                    while (!cbor_value_string_iteration_at_end(&strit) && chunk_steps++ < 64) {
                        size_t len = 0;
                        (void)cbor_value_get_string_chunk_size(&strit, &len);
                        (void)cbor_value_reparse(&strit);

                        if (!cbor_value_is_valid(&strit))
                            break;

                        if (is_fixed_type_safe(&strit)) {
                            err = cbor_value_advance_fixed(&strit);
                        } else {
                            err = cbor_value_advance(&strit);
                        }

                        if (err != CborNoError)
                            break;
                    }

                    if (cbor_value_string_iteration_at_end(&strit))
                        (void)cbor_value_finish_string_iteration(&strit);
                    else
                        (void)_cbor_value_finish_string_iteration(&strit);
                }
            }

            if (cbor_value_advance(&cur) != CborNoError)
                break;
            continue;
        }

        if (is_fixed_type_safe(&cur)) {
            if (cbor_value_advance_fixed(&cur) != CborNoError)
                break;
        } else {
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    *it = cur;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data != NULL && Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        (void)cbor_value_reparse(&it);
        (void)cbor_value_validate_basic(&it);
        exercise_value(&it);
    }

    if (Size != 0) {
        uint8_t *copy = (uint8_t *)malloc(Size);
        if (copy != NULL) {
            memcpy(copy, Data, Size);
            err = cbor_parser_init(copy, Size, 0, &parser, &it);
            if (err == CborNoError) {
                (void)cbor_value_reparse(&it);
                (void)cbor_value_validate_basic(&it);
                exercise_value(&it);
            }
            free(copy);
        }
    }

    return 0;
}