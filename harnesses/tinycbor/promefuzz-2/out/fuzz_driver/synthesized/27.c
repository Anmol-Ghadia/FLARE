// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include "cbor.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *off) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4; ++i) {
        v <<= 8;
        if (*off < size)
            v |= data[(*off)++];
    }
    return v;
}

static int is_fixed_width_type(const CborValue *it) {
    switch (it->type) {
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

static int is_string_like_type(const CborValue *it) {
    return it->type == CborByteStringType || it->type == CborTextStringType;
}

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    size_t off = 0;
    uint32_t flags = 0;
    int i;

    if (Size >= 4)
        flags = read_u32(Data, Size, &off);

    flags = 0;

    err = cbor_parser_init(Data + off, Size - off, flags, &parser, &it);
    (void)cbor_error_string(err);

    (void)cbor_error_string(CborNoError);
    (void)cbor_error_string(CborUnknownError);
    (void)cbor_error_string(CborErrorUnknownLength);
    (void)cbor_error_string(CborErrorAdvancePastEOF);
    (void)cbor_error_string(CborErrorIO);
    (void)cbor_error_string(CborErrorGarbageAtEnd);
    (void)cbor_error_string(CborErrorUnexpectedEOF);
    (void)cbor_error_string(CborErrorUnexpectedBreak);
    (void)cbor_error_string(CborErrorUnknownType);
    (void)cbor_error_string(CborErrorIllegalType);
    (void)cbor_error_string(CborErrorIllegalNumber);
    (void)cbor_error_string(CborErrorIllegalSimpleType);
    (void)cbor_error_string(CborErrorNoMoreStringChunks);
    (void)cbor_error_string(CborErrorUnknownSimpleType);
    (void)cbor_error_string(CborErrorUnknownTag);
    (void)cbor_error_string(CborErrorInappropriateTagForType);
    (void)cbor_error_string(CborErrorDuplicateObjectKeys);
    (void)cbor_error_string(CborErrorInvalidUtf8TextString);
    (void)cbor_error_string(CborErrorExcludedType);
    (void)cbor_error_string(CborErrorExcludedValue);
    (void)cbor_error_string(CborErrorImproperValue);
    (void)cbor_error_string(CborErrorOverlongEncoding);
    (void)cbor_error_string(CborErrorMapKeyNotString);
    (void)cbor_error_string(CborErrorMapNotSorted);
    (void)cbor_error_string(CborErrorMapKeysNotUnique);
    (void)cbor_error_string(CborErrorTooManyItems);
    (void)cbor_error_string(CborErrorTooFewItems);
    (void)cbor_error_string(CborErrorDataTooLarge);
    (void)cbor_error_string(CborErrorNestingTooDeep);
    (void)cbor_error_string(CborErrorUnsupportedType);
    (void)cbor_error_string(CborErrorUnimplementedValidation);
    (void)cbor_error_string(CborErrorJsonObjectKeyIsAggregate);
    (void)cbor_error_string(CborErrorJsonObjectKeyNotString);
    (void)cbor_error_string(CborErrorJsonNotImplemented);
    (void)cbor_error_string(CborErrorOutOfMemory);
    (void)cbor_error_string(CborErrorInternalError);
    (void)cbor_error_string((CborError)0x7fffffff);
    (void)cbor_error_string((CborError)-1);

    if (err != CborNoError)
        return 0;

    for (i = 0; i < 32; ++i) {
        CborValue tmp = it;
        CborError e;

        e = cbor_value_reparse(&tmp);
        (void)cbor_error_string(e);

        e = cbor_value_skip_tag(&tmp);
        (void)cbor_error_string(e);

        e = cbor_value_reparse(&tmp);
        (void)cbor_error_string(e);

        if (is_fixed_width_type(&tmp)) {
            e = cbor_value_advance_fixed(&tmp);
            (void)cbor_error_string(e);
        }

        /* Avoid calling _cbor_value_finish_string_iteration() on arbitrary
           string iterators because it is only safe after a valid string/chunk
           iteration state has been established by the proper string-reading APIs. */
        if (is_string_like_type(&tmp)) {
            CborValue tmp2 = tmp;
            (void)tmp2;
        }

        e = cbor_value_skip_tag(&it);
        (void)cbor_error_string(e);
        if (e != CborNoError)
            break;

        e = cbor_value_reparse(&it);
        (void)cbor_error_string(e);
        if (e != CborNoError)
            break;

        if (!is_fixed_width_type(&it))
            break;

        e = cbor_value_advance_fixed(&it);
        (void)cbor_error_string(e);
        if (e != CborNoError)
            break;
    }

    return 0;
}