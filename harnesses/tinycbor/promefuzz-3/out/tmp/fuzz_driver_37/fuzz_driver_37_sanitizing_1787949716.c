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
    if (!it)
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_reparse(it);

    if (cbor_value_at_end(it))
        return;

    if (cbor_value_is_length_known(it)) {
        if (is_fixed_type_safe(it)) {
            CborValue tmp = *it;
            (void)cbor_value_advance_fixed(&tmp);
        } else {
            CborValue tmp = *it;
            (void)cbor_value_advance(&tmp);
        }
    } else {
        if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) {
            CborValue chunk = *it;
            CborError err = cbor_value_begin_string_iteration(&chunk);
            if (err == CborNoError) {
                for (int i = 0; i < 32; ++i) {
                    size_t len = 0;
                    (void)cbor_value_get_string_chunk_size(&chunk, &len);
                    if (cbor_value_string_iteration_at_end(&chunk))
                        break;
                    err = cbor_value_advance(&chunk);
                    if (err != CborNoError)
                        break;
                }
                if (cbor_value_string_iteration_at_end(&chunk)) {
                    (void)cbor_value_finish_string_iteration(&chunk);
                    (void)_cbor_value_finish_string_iteration(&chunk);
                } else {
                    (void)_cbor_value_finish_string_iteration(&chunk);
                }
            }
        } else {
            CborValue tmp = *it;
            (void)cbor_value_advance(&tmp);
        }
    }

    if (cbor_value_is_container(it)) {
        CborValue rec;
        CborError err = cbor_value_enter_container(it, &rec);
        if (err == CborNoError) {
            for (int i = 0; i < 16 && !cbor_value_at_end(&rec); ++i) {
                exercise_value(&rec);
                if (cbor_value_at_end(&rec))
                    break;
                err = cbor_value_advance(&rec);
                if (err != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
    } else if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) {
        CborValue chunk = *it;
        CborError err = cbor_value_begin_string_iteration(&chunk);
        if (err == CborNoError) {
            for (int i = 0; i < 32; ++i) {
                size_t len = 0;
                (void)cbor_value_get_string_chunk_size(&chunk, &len);
                if (cbor_value_string_iteration_at_end(&chunk))
                    break;
                err = cbor_value_advance(&chunk);
                if (err != CborNoError)
                    break;
            }
            if (cbor_value_string_iteration_at_end(&chunk)) {
                (void)cbor_value_finish_string_iteration(&chunk);
            } else {
                (void)_cbor_value_finish_string_iteration(&chunk);
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    (void)cbor_value_validate_basic(&it);
    (void)cbor_value_reparse(&it);

    CborValue cur = it;
    for (int i = 0; i < 32 && !cbor_value_at_end(&cur); ++i) {
        exercise_value(&cur);
        if (cbor_value_at_end(&cur))
            break;

        if (is_fixed_type_safe(&cur)) {
            err = cbor_value_advance_fixed(&cur);
            if (err != CborNoError)
                break;
        } else {
            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }

        (void)cbor_value_reparse(&cur);
        (void)cbor_value_validate_basic(&cur);
    }

    return 0;
}