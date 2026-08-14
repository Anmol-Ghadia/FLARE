#include "cbor.h"

#include <stdint.h>
#include <stdlib.h>

enum {
    kMaxDepth = 64,
    kMaxItems = 4096,
    kMaxChunks = 4096,
    kMaxTopLevelValues = 256
};

static const uint32_t kValidationFlags[] = {
    CborValidateBasic,
    CborValidateCanonicalFormat,
    CborValidateMapKeysAreUnique | CborValidateUtf8 | CborValidateCompleteData,
    CborValidateStrictMode | CborValidateCompleteData,
    CborValidateStrictest
};

static bool is_fixed_type(CborType type)
{
    switch (type) {
    case CborIntegerType:
    case CborTagType:
    case CborSimpleType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
        return true;
    default:
        return false;
    }
}

static void exercise_string_chunks(CborValue *value)
{
    CborValue chunks = *value;
    size_t i;

    if (cbor_value_begin_string_iteration(&chunks) != CborNoError)
        return;

    for (i = 0; i < kMaxChunks; ++i) {
        CborError err;
        size_t len = 0;

        if (cbor_value_is_text_string(&chunks)) {
            const char *buffer = NULL;
            err = cbor_value_get_text_string_chunk(&chunks, &buffer, &len, &chunks);
            (void)buffer;
        } else {
            const uint8_t *buffer = NULL;
            err = cbor_value_get_byte_string_chunk(&chunks, &buffer, &len, &chunks);
            (void)buffer;
        }

        if (err == CborErrorNoMoreStringChunks)
            break;
        if (err != CborNoError)
            return;
    }

    if (cbor_value_string_iteration_at_end(&chunks))
        (void)cbor_value_finish_string_iteration(&chunks);
}

static void exercise_string(CborValue *value)
{
    size_t len = 0;

    (void)cbor_value_get_string_length(value, &len);
    (void)cbor_value_calculate_string_length(value, &len);

    if (cbor_value_is_text_string(value)) {
        bool equals = false;
        char *buffer = NULL;
        size_t copied = 0;

        (void)cbor_value_text_string_equals(value, "", &equals);
        (void)cbor_value_text_string_equals(value, "needle", &equals);
        if (cbor_value_dup_text_string(value, &buffer, &copied, NULL) == CborNoError) {
            (void)cbor_value_text_string_equals(value, buffer, &equals);
            free(buffer);
        }
    } else {
        uint8_t *buffer = NULL;
        size_t copied = 0;

        if (cbor_value_dup_byte_string(value, &buffer, &copied, NULL) == CborNoError)
            free(buffer);
    }

    exercise_string_chunks(value);
}

static CborError walk_stream(CborValue *value, int depth);

static CborError walk_value(CborValue *value, int depth)
{
    CborType type = cbor_value_get_type(value);

    switch (type) {
    case CborIntegerType: {
        uint64_t raw = 0;
        int64_t i64 = 0;
        int i = 0;

        (void)cbor_value_get_raw_integer(value, &raw);
        (void)cbor_value_get_int64(value, &i64);
        (void)cbor_value_get_int64_checked(value, &i64);
        (void)cbor_value_get_int(value, &i);
        (void)cbor_value_get_int_checked(value, &i);
        if (cbor_value_is_unsigned_integer(value)) {
            uint64_t u64 = 0;
            (void)cbor_value_get_uint64(value, &u64);
        }
        break;
    }
    case CborByteStringType:
    case CborTextStringType:
        exercise_string(value);
        break;
    case CborArrayType: {
        size_t len = 0;
        (void)cbor_value_get_array_length(value, &len);
        break;
    }
    case CborMapType: {
        CborValue element;
        size_t len = 0;

        (void)cbor_value_get_map_length(value, &len);
        (void)cbor_value_map_find_value(value, "", &element);
        (void)cbor_value_map_find_value(value, "needle", &element);
        break;
    }
    case CborTagType: {
        CborTag tag = 0;
        (void)cbor_value_get_tag(value, &tag);
        if (depth > 0) {
            CborValue tagged = *value;
            if (cbor_value_skip_tag(&tagged) == CborNoError)
                (void)walk_stream(&tagged, depth - 1);
        }
        break;
    }
    case CborSimpleType: {
        uint8_t simple = 0;
        (void)cbor_value_get_simple_type(value, &simple);
        break;
    }
    case CborBooleanType: {
        bool boolean = false;
        (void)cbor_value_get_boolean(value, &boolean);
        break;
    }
    case CborHalfFloatType: {
        uint16_t raw = 0;
        float half = 0.0f;

        (void)cbor_value_get_half_float(value, &raw);
        (void)cbor_value_get_half_float_as_float(value, &half);
        break;
    }
    case CborFloatType: {
        float fp = 0.0f;
        (void)cbor_value_get_float(value, &fp);
        break;
    }
    case CborDoubleType: {
        double fp = 0.0;
        (void)cbor_value_get_double(value, &fp);
        break;
    }
    case CborNullType:
    case CborUndefinedType:
    case CborInvalidType:
        break;
    }

    if (cbor_value_is_container(value)) {
        CborValue recursed;
        CborError err;

        if (depth == 0)
            return cbor_value_advance(value);

        err = cbor_value_enter_container(value, &recursed);
        if (err != CborNoError)
            return err;
        err = walk_stream(&recursed, depth - 1);
        if (err != CborNoError)
            return err;
        return cbor_value_leave_container(value, &recursed);
    }

    if (is_fixed_type(type))
        return cbor_value_advance_fixed(value);
    return cbor_value_advance(value);
}

static CborError walk_stream(CborValue *value, int depth)
{
    size_t i;

    for (i = 0; i < kMaxItems && !cbor_value_at_end(value); ++i) {
        CborError err = walk_value(value, depth);
        if (err != CborNoError)
            return err;
    }

    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const uint8_t *cursor = data;
    size_t remaining = size;
    size_t i;

    for (i = 0; i < kMaxTopLevelValues && remaining != 0; ++i) {
        const uint8_t *next;
        CborParser parser;
        CborValue value;
        CborValue copy;
        size_t j;

        if (cbor_parser_init(cursor, remaining, 0, &parser, &value) != CborNoError)
            break;

        (void)cbor_value_validate_basic(&value);
        for (j = 0; j < sizeof(kValidationFlags) / sizeof(kValidationFlags[0]); ++j)
            (void)cbor_value_validate(&value, kValidationFlags[j]);

        copy = value;
        (void)walk_stream(&copy, kMaxDepth);
        next = cbor_value_get_next_byte(&copy);
        if (next <= cursor || (size_t)(next - cursor) > remaining)
            break;

        remaining -= (size_t)(next - cursor);
        cursor = next;
    }

    return 0;
}
