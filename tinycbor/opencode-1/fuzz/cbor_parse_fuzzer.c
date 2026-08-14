#include "cbor.h"

#include <stdint.h>
#include <stdlib.h>

static void exercise_string_chunks(const CborValue *value)
{
    CborValue chunks = *value;
    CborError err = cbor_value_begin_string_iteration(&chunks);
    if (err != CborNoError)
        return;

    for (;;) {
        CborValue next;
        size_t len = 0;

        if (cbor_value_is_text_string(&chunks)) {
            const char *text = NULL;
            err = cbor_value_get_text_string_chunk(&chunks, &text, &len, &next);
        } else {
            const uint8_t *bytes = NULL;
            err = cbor_value_get_byte_string_chunk(&chunks, &bytes, &len, &next);
        }

        if (err == CborErrorNoMoreStringChunks)
            break;
        if (err != CborNoError)
            return;

        chunks = next;
    }

    (void)cbor_value_finish_string_iteration(&chunks);
}

static void walk_value(CborValue *it, int depth)
{
    while (!cbor_value_at_end(it)) {
        CborValue reparsed = *it;
        (void)cbor_value_reparse(&reparsed);

        switch (cbor_value_get_type(it)) {
        case CborArrayType:
        case CborMapType:
            if (cbor_value_is_map(it)) {
                CborValue element;
                (void)cbor_value_map_find_value(it, "needle", &element);
            }

            if (depth < 64) {
                CborValue recursed;
                if (cbor_value_enter_container(it, &recursed) == CborNoError) {
                    walk_value(&recursed, depth + 1);
                    if (cbor_value_leave_container(it, &recursed) == CborNoError)
                        continue;
                }
            }
            break;

        case CborIntegerType: {
            int int_value;
            int64_t int64_value;
            (void)cbor_value_get_int_checked(it, &int_value);
            (void)cbor_value_get_int64_checked(it, &int64_value);
            break;
        }

        case CborByteStringType: {
            uint8_t *buffer = NULL;
            CborValue next;
            size_t len = 0;

            (void)cbor_value_calculate_string_length(it, &len);
            exercise_string_chunks(it);
            if (cbor_value_dup_byte_string(it, &buffer, &len, &next) == CborNoError) {
                free(buffer);
                *it = next;
                continue;
            }
            break;
        }

        case CborTextStringType: {
            char *buffer = NULL;
            bool equal = false;
            CborValue next;
            size_t len = 0;

            (void)cbor_value_calculate_string_length(it, &len);
            (void)cbor_value_text_string_equals(it, "needle", &equal);
            exercise_string_chunks(it);
            if (cbor_value_dup_text_string(it, &buffer, &len, &next) == CborNoError) {
                free(buffer);
                *it = next;
                continue;
            }
            break;
        }

        case CborTagType: {
            CborTag tag;
            CborValue tagged = *it;
            (void)cbor_value_get_tag(it, &tag);
            (void)cbor_value_skip_tag(&tagged);
            break;
        }

        case CborSimpleType: {
            uint8_t simple_type;
            (void)cbor_value_get_simple_type(it, &simple_type);
            break;
        }

        case CborBooleanType: {
            bool boolean_value;
            (void)cbor_value_get_boolean(it, &boolean_value);
            break;
        }

        case CborHalfFloatType: {
            float float_value;
            uint16_t half_value;
            (void)cbor_value_get_half_float(it, &half_value);
            (void)cbor_value_get_half_float_as_float(it, &float_value);
            break;
        }

        case CborFloatType: {
            float float_value;
            (void)cbor_value_get_float(it, &float_value);
            break;
        }

        case CborDoubleType: {
            double double_value;
            (void)cbor_value_get_double(it, &double_value);
            break;
        }

        case CborNullType:
        case CborUndefinedType:
        case CborInvalidType:
            break;
        }

        if (cbor_value_advance(it) != CborNoError)
            return;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    CborError err;
    CborParser parser;
    CborValue value;

    err = cbor_parser_init(data, size, 0, &parser, &value);
    if (err != CborNoError) {
        (void)cbor_error_string(err);
        return 0;
    }

    (void)cbor_value_validate_basic(&value);
    (void)cbor_value_validate(&value, CborValidateBasic);
    (void)cbor_value_validate(&value, CborValidateCompleteData);
    (void)cbor_value_validate(&value, CborValidateStrictest);

    walk_value(&value, 0);
    return 0;
}
