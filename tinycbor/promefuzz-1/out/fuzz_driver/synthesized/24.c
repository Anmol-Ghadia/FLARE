// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_undefined at cbor.h:259:27 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_undefined at cbor.h:259:27 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_encode_int at cborencoder.c:372:11 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
// cbor_error_string at cborerrorstrings.c:80:13 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "cbor.h"

static uint8_t consume_u8(const uint8_t **data, size_t *size) {
    if (*size == 0)
        return 0;
    uint8_t v = **data;
    (*data)++;
    (*size)--;
    return v;
}

static uint32_t consume_u32(const uint8_t **data, size_t *size) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = (v << 8) | consume_u8(data, size);
    }
    return v;
}

static uint64_t consume_u64(const uint8_t **data, size_t *size) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) {
        v = (v << 8) | consume_u8(data, size);
    }
    return v;
}

static int64_t consume_i64(const uint8_t **data, size_t *size) {
    return (int64_t)consume_u64(data, size);
}

static void exercise_error_strings(void) {
    static const CborError errors[] = {
        CborNoError,
        CborUnknownError,
        CborErrorUnknownLength,
        CborErrorAdvancePastEOF,
        CborErrorIO,
        CborErrorGarbageAtEnd,
        CborErrorUnexpectedEOF,
        CborErrorUnexpectedBreak,
        CborErrorUnknownType,
        CborErrorIllegalType,
        CborErrorIllegalNumber,
        CborErrorIllegalSimpleType,
        CborErrorNoMoreStringChunks,
        CborErrorUnknownSimpleType,
        CborErrorUnknownTag,
        CborErrorInappropriateTagForType,
        CborErrorDuplicateObjectKeys,
        CborErrorInvalidUtf8TextString,
        CborErrorExcludedType,
        CborErrorExcludedValue,
        CborErrorImproperValue,
        CborErrorOverlongEncoding,
        CborErrorMapKeyNotString,
        CborErrorMapNotSorted,
        CborErrorMapKeysNotUnique,
        CborErrorTooManyItems,
        CborErrorTooFewItems,
        CborErrorDataTooLarge,
        CborErrorNestingTooDeep,
        CborErrorUnsupportedType,
        CborErrorUnimplementedValidation,
        CborErrorJsonObjectKeyIsAggregate,
        CborErrorJsonObjectKeyNotString,
        CborErrorJsonNotImplemented,
        CborErrorOutOfMemory,
        CborErrorInternalError
    };

    for (size_t i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
        const char *s = cbor_error_string(errors[i]);
        (void)s;
    }

    (void)cbor_error_string((CborError)-1);
    (void)cbor_error_string((CborError)12345);
    (void)cbor_error_string((CborError)INT_MAX);
    (void)cbor_error_string((CborError)INT_MIN);
}

int LLVMFuzzerTestOneInput_24(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t remaining = Size;

    exercise_error_strings();

    uint8_t stack_buf[256];
    memset(stack_buf, 0, sizeof(stack_buf));

    CborEncoder encoder;
    size_t buf_size = remaining ? (size_t)(consume_u8(&p, &remaining) % sizeof(stack_buf)) : 0;

    cbor_encoder_init(&encoder, stack_buf, buf_size, 0);

    for (int i = 0; i < 32 && remaining > 0; i++) {
        uint8_t op = consume_u8(&p, &remaining) % 8;
        CborError err = CborNoError;

        switch (op) {
        case 0: {
            err = cbor_encode_null(&encoder);
            break;
        }
        case 1: {
            uint8_t simple = consume_u8(&p, &remaining);
            err = cbor_encode_simple_value(&encoder, simple);
            break;
        }
        case 2: {
            int64_t v = consume_i64(&p, &remaining);
            err = cbor_encode_int(&encoder, v);
            break;
        }
        case 3: {
            bool b = (consume_u8(&p, &remaining) & 1) != 0;
            err = cbor_encode_boolean(&encoder, b);
            break;
        }
        case 4: {
            err = cbor_encode_undefined(&encoder);
            break;
        }
        case 5: {
            uint8_t simple = consume_u8(&p, &remaining);
            err = cbor_encode_simple_value(&encoder, simple & 0x1f);
            break;
        }
        case 6: {
            uint32_t selector = consume_u32(&p, &remaining);
            int64_t v;
            switch (selector % 8) {
            case 0: v = 0; break;
            case 1: v = -1; break;
            case 2: v = 1; break;
            case 3: v = INT64_MAX; break;
            case 4: v = INT64_MIN; break;
            case 5: v = 23; break;
            case 6: v = 24; break;
            default: v = -24; break;
            }
            err = cbor_encode_int(&encoder, v);
            break;
        }
        case 7: {
            CborError e = (CborError)consume_u32(&p, &remaining);
            const char *s = cbor_error_string(e);
            (void)s;
            continue;
        }
        }

        const char *msg = cbor_error_string(err);
        (void)msg;
    }

    {
        CborEncoder zero_encoder;
        cbor_encoder_init(&zero_encoder, stack_buf, 0, 0);
        (void)cbor_error_string(cbor_encode_null(&zero_encoder));
        (void)cbor_error_string(cbor_encode_boolean(&zero_encoder, false));
        (void)cbor_error_string(cbor_encode_boolean(&zero_encoder, true));
        (void)cbor_error_string(cbor_encode_undefined(&zero_encoder));
        (void)cbor_error_string(cbor_encode_simple_value(&zero_encoder, 0));
        (void)cbor_error_string(cbor_encode_simple_value(&zero_encoder, 255));
        (void)cbor_error_string(cbor_encode_int(&zero_encoder, 0));
        (void)cbor_error_string(cbor_encode_int(&zero_encoder, -1));
    }

    return 0;
}