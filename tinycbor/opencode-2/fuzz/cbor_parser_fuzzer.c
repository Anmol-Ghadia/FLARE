#include <stdint.h>
#include <stdlib.h>

#include "cbor.h"

enum {
    kMaxDepth = 64,
    kMaxStringAllocation = 1 << 20
};

static void exercise_string_value(const CborValue *value, size_t input_size)
{
    CborError err;
    size_t length = 0;

    if (cbor_value_is_length_known(value))
        (void)cbor_value_get_string_length(value, &length);

    err = cbor_value_calculate_string_length(value, &length);
    if (err == CborNoError && length <= input_size && length <= kMaxStringAllocation) {
        if (cbor_value_is_byte_string(value)) {
            uint8_t *buffer = (uint8_t *)malloc(length + 1);
            uint8_t *duplicate = NULL;
            if (buffer != NULL) {
                size_t buffer_size = length + 1;
                (void)cbor_value_copy_byte_string(value, buffer, &buffer_size, NULL);
                free(buffer);
            }

            if (cbor_value_dup_byte_string(value, &duplicate, &length, NULL) == CborNoError)
                free(duplicate);
        } else {
            char *buffer = (char *)malloc(length + 1);
            char *duplicate = NULL;
            bool equals = false;
            if (buffer != NULL) {
                size_t buffer_size = length + 1;
                (void)cbor_value_copy_text_string(value, buffer, &buffer_size, NULL);
                free(buffer);
            }

            (void)cbor_value_text_string_equals(value, "", &equals);
            (void)cbor_value_text_string_equals(value, "needle", &equals);
            (void)equals;

            if (cbor_value_dup_text_string(value, &duplicate, &length, NULL) == CborNoError)
                free(duplicate);
        }
    }

    {
        CborValue copy = *value;
        err = cbor_value_begin_string_iteration(&copy);
        if (err != CborNoError)
            return;

        for (;;) {
            size_t chunk_length = 0;
            if (cbor_value_is_byte_string(&copy)) {
                const uint8_t *chunk = NULL;
                err = cbor_value_get_byte_string_chunk(&copy, &chunk, &chunk_length, &copy);
                (void)chunk;
            } else {
                const char *chunk = NULL;
                err = cbor_value_get_text_string_chunk(&copy, &chunk, &chunk_length, &copy);
                (void)chunk;
            }
            (void)chunk_length;

            if (err == CborErrorNoMoreStringChunks)
                break;
            if (err != CborNoError)
                return;
        }

        if (cbor_value_string_iteration_at_end(&copy))
            (void)cbor_value_finish_string_iteration(&copy);
    }
}

static CborError walk_value(CborValue *value, size_t input_size, unsigned depth)
{
    while (!cbor_value_at_end(value)) {
        CborError err;
        CborValue reparsed = *value;
        (void)cbor_value_reparse(&reparsed);

        switch (cbor_value_get_type(value)) {
        case CborIntegerType: {
            uint64_t raw = 0;
            uint64_t unsigned_value = 0;
            int64_t int64_value = 0;
            int int_value = 0;

            (void)cbor_value_get_raw_integer(value, &raw);
            (void)cbor_value_get_int64(value, &int64_value);
            (void)cbor_value_get_int(value, &int_value);
            (void)cbor_value_get_int64_checked(value, &int64_value);
            (void)cbor_value_get_int_checked(value, &int_value);
            if (cbor_value_is_unsigned_integer(value))
                (void)cbor_value_get_uint64(value, &unsigned_value);

            (void)raw;
            (void)unsigned_value;
            (void)int64_value;
            (void)int_value;
            break;
        }

        case CborByteStringType:
        case CborTextStringType:
            exercise_string_value(value, input_size);
            break;

        case CborArrayType:
        case CborMapType: {
            size_t length = 0;

            if (cbor_value_is_length_known(value)) {
                if (cbor_value_is_array(value))
                    (void)cbor_value_get_array_length(value, &length);
                else
                    (void)cbor_value_get_map_length(value, &length);
            }
            (void)length;

            if (cbor_value_is_map(value) && depth < 8) {
                CborValue element;
                (void)cbor_value_map_find_value(value, "", &element);
                (void)cbor_value_map_find_value(value, "id", &element);
                (void)cbor_value_map_find_value(value, "needle", &element);
            }

            if (depth >= kMaxDepth) {
                err = cbor_value_advance(value);
                if (err != CborNoError)
                    return err;
                continue;
            }

            {
                CborValue recursed;
                err = cbor_value_enter_container(value, &recursed);
                if (err != CborNoError)
                    return err;

                err = walk_value(&recursed, input_size, depth + 1);
                if (err != CborNoError)
                    return err;

                err = cbor_value_leave_container(value, &recursed);
                if (err != CborNoError)
                    return err;
            }
            continue;
        }

        case CborTagType: {
            CborTag tag = 0;
            bool equals = false;
            (void)cbor_value_get_tag(value, &tag);
            (void)cbor_value_text_string_equals(value, "needle", &equals);
            (void)tag;
            (void)equals;
            break;
        }

        case CborSimpleType: {
            uint8_t simple = 0;
            (void)cbor_value_get_simple_type(value, &simple);
            (void)simple;
            break;
        }

        case CborBooleanType: {
            bool result = false;
            (void)cbor_value_get_boolean(value, &result);
            (void)result;
            break;
        }

        case CborHalfFloatType: {
            uint16_t raw = 0;
            float result = 0.0f;
            (void)cbor_value_get_half_float(value, &raw);
            (void)cbor_value_get_half_float_as_float(value, &result);
            (void)raw;
            (void)result;
            break;
        }

        case CborFloatType: {
            float result = 0.0f;
            (void)cbor_value_get_float(value, &result);
            (void)result;
            break;
        }

        case CborDoubleType: {
            double result = 0.0;
            (void)cbor_value_get_double(value, &result);
            (void)result;
            break;
        }

        case CborNullType:
        case CborUndefinedType:
            break;

        case CborInvalidType:
            return CborNoError;
        }

        err = cbor_value_advance(value);
        if (err != CborNoError)
            return err;
    }

    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    CborParser parser;
    CborValue value;

    if (cbor_parser_init(data, size, 0, &parser, &value) != CborNoError)
        return 0;

    (void)cbor_value_validate_basic(&value);
    (void)cbor_value_validate(&value, CborValidateBasic);
    (void)cbor_value_validate(&value, CborValidateCanonicalFormat);
    (void)cbor_value_validate(&value, (uint32_t)CborValidateStrictest);

    (void)walk_value(&value, size, 0);
    return 0;
}
