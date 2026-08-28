#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "cbor.h"

static int is_fixed_type_safe(const CborValue *it) {
    CborType t = cbor_value_get_type(it);
    switch (t) {
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

static int type_is_known(CborType t) {
    switch (t) {
    case CborIntegerType:
    case CborByteStringType:
    case CborTextStringType:
    case CborArrayType:
    case CborMapType:
    case CborTagType:
    case CborSimpleType:
    case CborBooleanType:
    case CborNullType:
    case CborUndefinedType:
    case CborHalfFloatType:
    case CborFloatType:
    case CborDoubleType:
    case CborInvalidType:
        return 1;
    default:
        return 0;
    }
}

static int value_seems_usable(const CborValue *it) {
    if (!it || !it->parser)
        return 0;
    return type_is_known(cbor_value_get_type(it));
}

static void exercise_value(CborValue *it) {
    if (!value_seems_usable(it))
        return;

    for (int steps = 0; steps < 32; ++steps) {
        if (!value_seems_usable(it))
            break;
        if (cbor_value_at_end(it))
            break;

        CborType t = cbor_value_get_type(it);
        if (t == CborInvalidType)
            break;

        if (t == CborByteStringType || t == CborTextStringType) {
            CborValue chunk = *it;
            CborError err = cbor_value_begin_string_iteration(&chunk);
            if (err == CborNoError) {
                for (int i = 0; i < 32; ++i) {
                    if (!value_seems_usable(&chunk))
                        break;
                    if (cbor_value_at_end(&chunk))
                        break;
                    if (cbor_value_string_iteration_at_end(&chunk))
                        break;

                    size_t len = 0;
                    (void)cbor_value_get_string_chunk_size(&chunk, &len);

                    if (!value_seems_usable(&chunk))
                        break;
                    if (cbor_value_at_end(&chunk))
                        break;

                    CborType ct = cbor_value_get_type(&chunk);
                    if (ct == CborInvalidType)
                        break;

                    CborError aerr;
                    if (is_fixed_type_safe(&chunk))
                        aerr = cbor_value_advance_fixed(&chunk);
                    else
                        aerr = cbor_value_advance(&chunk);

                    if (aerr != CborNoError)
                        break;
                }

                if (value_seems_usable(&chunk) && cbor_value_string_iteration_at_end(&chunk))
                    (void)cbor_value_finish_string_iteration(&chunk);

                if (value_seems_usable(&chunk))
                    *it = chunk;
                else
                    break;

                if (!value_seems_usable(it))
                    break;

                (void)cbor_value_reparse(it);
                continue;
            }
        }

        (void)cbor_value_reparse(it);
        if (!value_seems_usable(it))
            break;
        if (cbor_value_at_end(it))
            break;

        t = cbor_value_get_type(it);
        if (t == CborInvalidType)
            break;

        (void)cbor_value_validate_basic(it);

        CborError adv;
        if (is_fixed_type_safe(it))
            adv = cbor_value_advance_fixed(it);
        else
            adv = cbor_value_advance(it);

        if (adv != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError && value_seems_usable(&it)) {
        CborValue copy1 = it;
        CborValue copy2 = it;
        exercise_value(&copy1);
        exercise_value(&copy2);
    }

    if (Size > 0) {
        uint8_t local[4096];
        size_t n = Size < sizeof(local) ? Size : sizeof(local);
        memcpy(local, Data, n);

        local[0] ^= 0x1f;
        if (n > 1)
            local[n - 1] ^= 0x80;

        err = cbor_parser_init(local, n, 0, &parser, &it);
        if (err == CborNoError && value_seems_usable(&it))
            exercise_value(&it);
    }

    return 0;
}