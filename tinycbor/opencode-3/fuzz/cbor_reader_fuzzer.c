#include "cbor.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct ReaderInput {
    const uint8_t *data;
    size_t size;
    size_t consumed;
} ReaderInput;

static bool reader_can_read_bytes(void *token, size_t len)
{
    ReaderInput *input = (ReaderInput *)token;
    return input->consumed <= input->size && len <= input->size - input->consumed;
}

static void *reader_read_bytes(void *token, void *dst, size_t offset, size_t len)
{
    ReaderInput *input = (ReaderInput *)token;
    return memcpy(dst, input->data + input->consumed + offset, len);
}

static void reader_advance_bytes(void *token, size_t len)
{
    ReaderInput *input = (ReaderInput *)token;
    input->consumed += len;
}

static CborError reader_transfer_string(void *token, const void **userptr,
                                        size_t offset, size_t len)
{
    ReaderInput *input = (ReaderInput *)token;

    if (input->consumed > input->size)
        return CborErrorUnexpectedEOF;
    if (offset > input->size - input->consumed)
        return CborErrorUnexpectedEOF;
    if (len > input->size - input->consumed - offset)
        return CborErrorUnexpectedEOF;

    input->consumed += offset;
    *userptr = input->data + input->consumed;
    input->consumed += len;
    return CborNoError;
}

static const struct CborParserOperations kReaderOps = {
    reader_can_read_bytes,
    reader_read_bytes,
    reader_advance_bytes,
    reader_transfer_string
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

static CborError consume_byte_string(CborValue *value)
{
    size_t len = 0;
    size_t i;
    CborError err;

    (void)cbor_value_get_string_length(value, &len);
    err = cbor_value_begin_string_iteration(value);
    if (err != CborNoError)
        return err;

    for (i = 0; i < kMaxChunks; ++i) {
        const uint8_t *buffer = NULL;
        err = cbor_value_get_byte_string_chunk(value, &buffer, &len, value);
        (void)buffer;
        if (err == CborErrorNoMoreStringChunks)
            break;
        if (err != CborNoError)
            return err;
    }

    if (cbor_value_string_iteration_at_end(value))
        return cbor_value_finish_string_iteration(value);
    return CborUnknownError;
}

static CborError consume_text_string(CborValue *value)
{
    char *buffer = NULL;
    size_t len = 0;
    CborError err;

    (void)cbor_value_get_string_length(value, &len);
    err = cbor_value_dup_text_string(value, &buffer, &len, value);
    if (err == CborNoError)
        free(buffer);
    return err;
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
        return consume_byte_string(value);
    case CborTextStringType:
        return consume_text_string(value);
    case CborArrayType: {
        size_t len = 0;
        (void)cbor_value_get_array_length(value, &len);
        break;
    }
    case CborMapType: {
        size_t len = 0;
        (void)cbor_value_get_map_length(value, &len);
        break;
    }
    case CborTagType: {
        CborTag tag = 0;
        (void)cbor_value_get_tag(value, &tag);
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

static void exercise_validation(const uint8_t *data, size_t size, uint32_t flags)
{
    ReaderInput input = { data, size, 0 };
    CborParser parser;
    CborValue value;

    if (cbor_parser_init_reader(&kReaderOps, &parser, &value, &input) != CborNoError)
        return;
    (void)cbor_value_validate(&value, flags);
}

static void exercise_walk(const uint8_t *data, size_t size)
{
    const uint8_t *cursor = data;
    size_t remaining = size;
    size_t i;

    for (i = 0; i < kMaxTopLevelValues && remaining != 0; ++i) {
        ReaderInput input = { cursor, remaining, 0 };
        CborParser parser;
        CborValue value;

        if (cbor_parser_init_reader(&kReaderOps, &parser, &value, &input) != CborNoError)
            break;
        if (walk_stream(&value, kMaxDepth) != CborNoError)
            break;
        if (input.consumed == 0 || input.consumed > remaining)
            break;

        cursor += input.consumed;
        remaining -= input.consumed;
    }
}

static void exercise_reparse(const uint8_t *data, size_t size)
{
    ReaderInput input;
    CborParser parser;
    CborValue value;

    if (size == 0)
        return;

    input.data = data;
    input.size = size - 1;
    input.consumed = 0;

    if (cbor_parser_init_reader(&kReaderOps, &parser, &value, &input) != CborErrorUnexpectedEOF)
        return;

    input.size = size;
    input.consumed = 0;
    if (cbor_value_reparse(&value) != CborNoError)
        return;

    (void)walk_stream(&value, kMaxDepth);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    size_t i;

    exercise_walk(data, size);
    for (i = 0; i < sizeof(kValidationFlags) / sizeof(kValidationFlags[0]); ++i)
        exercise_validation(data, size, kValidationFlags[i]);
    exercise_reparse(data, size);
    return 0;
}
