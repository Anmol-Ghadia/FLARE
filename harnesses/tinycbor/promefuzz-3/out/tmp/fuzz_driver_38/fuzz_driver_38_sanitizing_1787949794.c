#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

static void exercise_value(CborValue *it) {
    if (!it)
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_reparse(it);

    for (int steps = 0; steps < 32 && !cbor_value_at_end(it); ++steps) {
        (void)cbor_value_validate_basic(it);
        (void)cbor_value_reparse(it);

        CborType t = cbor_value_get_type(it);

        if (t == CborByteStringType || t == CborTextStringType) {
            CborValue chunk = *it;
            CborError err = cbor_value_begin_string_iteration(it, &chunk);
            if (err == CborNoError) {
                for (int i = 0; i < 32; ++i) {
                    size_t len = 0;
                    (void)cbor_value_reparse(&chunk);

                    if (cbor_value_string_iteration_at_end(&chunk))
                        break;

                    (void)cbor_value_get_string_chunk_size(&chunk, &len);
                    (void)cbor_value_validate_basic(&chunk);

                    CborError aerr;
                    if (is_fixed_type_safe(&chunk))
                        aerr = cbor_value_advance_fixed(&chunk);
                    else
                        aerr = cbor_value_advance(&chunk);

                    if (aerr != CborNoError)
                        break;
                }

                if (cbor_value_string_iteration_at_end(&chunk))
                    (void)cbor_value_finish_string_iteration(&chunk);
                else
                    (void)_cbor_value_finish_string_iteration(&chunk);

                *it = chunk;
                continue;
            }
        }

        if (is_fixed_type_safe(it)) {
            if (cbor_value_advance_fixed(it) != CborNoError)
                break;
        } else {
            if (cbor_value_advance(it) != CborNoError)
                break;
        }
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
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        CborValue copy1 = it;
        CborValue copy2 = it;
        exercise_value(&copy1);
        exercise_value(&copy2);
    }

    if (Size > 0) {
        uint8_t local[4096];
        size_t n = Size < sizeof(local) ? Size : sizeof(local);
        memcpy(local, Data, n);

        if (n > 0)
            local[0] ^= 0x1f;
        if (n > 1)
            local[n - 1] ^= 0x80;

        err = cbor_parser_init(local, n, 0, &parser, &it);
        if (err == CborNoError) {
            (void)cbor_value_reparse(&it);
            exercise_value(&it);
        }
    }

    return 0;
}