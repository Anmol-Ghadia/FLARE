#include <stdint.h>
#include <stddef.h>
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

static void fuzz_one_mode(const uint8_t *Data, size_t Size, uint32_t flags) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, flags, &parser, &it);
    if (err != CborNoError)
        return;

    for (int steps = 0; steps < 64 && !cbor_value_at_end(&it); ++steps) {
        (void)cbor_value_validate_basic(&it);
        (void)cbor_value_reparse(&it);

        if (cbor_value_is_text_string(&it) || cbor_value_is_byte_string(&it)) {
            bool is_indef = cbor_value_is_length_known(&it) ? false : true;

            if (is_indef) {
                CborValue chunks = it;
                CborValue next = it;

                for (int chunk_steps = 0; chunk_steps < 32; ++chunk_steps) {
                    (void)cbor_value_reparse(&chunks);
                    (void)cbor_value_validate_basic(&chunks);

                    size_t chunk_len = 0;
                    CborError gr = cbor_value_get_string_chunk_size(&chunks, &chunk_len);
                    if (gr == CborErrorNoMoreStringChunks)
                        break;
                    if (gr != CborNoError)
                        break;

                    err = cbor_value_advance(&chunks);
                    if (err != CborNoError)
                        break;
                }

                (void)cbor_value_finish_string_iteration(&next);
                (void)_cbor_value_finish_string_iteration(&it);
                it = next;
                continue;
            }
        }

        if (is_fixed_type_safe(&it))
            err = cbor_value_advance_fixed(&it);
        else
            err = cbor_value_advance(&it);

        if (err != CborNoError)
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

    fuzz_one_mode(Data, Size, 0);
#ifdef CborValidateBasic
    fuzz_one_mode(Data, Size, CborValidateBasic);
#endif
#ifdef CborValidateCanonicalFormat
    fuzz_one_mode(Data, Size, CborValidateCanonicalFormat);
#endif
#ifdef CborValidateStrictMode
    fuzz_one_mode(Data, Size, CborValidateStrictMode);
#endif
    return 0;
}