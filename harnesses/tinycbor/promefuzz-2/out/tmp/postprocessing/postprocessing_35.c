#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static int can_use_value(const CborValue *it) {
    return it != NULL && it->parser != NULL;
}

static void exercise_value(const CborValue *it, int depth) {
    CborValue cur;
    CborError err;
    int guard = 0;

    if (!can_use_value(it) || depth > 8)
        return;

    cur = *it;

    (void)cbor_value_validate_basic(&cur);
    (void)cbor_value_validate(&cur, 0);
    (void)cbor_value_validate(&cur, CborValidateCompleteData);
    (void)cbor_value_reparse(&cur);

    if (!cbor_value_is_container(&cur))
        return;

    {
        CborValue recursed;
        err = cbor_value_enter_container(&cur, &recursed);
        if (err != CborNoError)
            return;

        while (!cbor_value_at_end(&recursed) && guard++ < 64) {
            CborValue child = recursed;

            if (!can_use_value(&child))
                break;

            (void)cbor_value_validate_basic(&child);
            (void)cbor_value_validate(&child, 0);
            (void)cbor_value_reparse(&child);

            exercise_value(&child, depth + 1);

            err = cbor_value_advance(&recursed);
            if (err != CborNoError)
                return;
        }

        if (cbor_value_at_end(&recursed))
            (void)cbor_value_leave_container(&cur, &recursed);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    uint32_t parser_flags[] = {
        0,
        CborValidateUtf8,
        CborValidateMapKeysAreString
    };
    uint32_t validate_flags[] = {
        0,
        CborValidateBasic,
        CborValidateCompleteData,
        CborValidateCanonicalFormat,
        CborValidateStrictMode,
        CborValidateStrictMode | CborValidateCompleteData
    };
    size_t i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    for (i = 0; i < sizeof(parser_flags) / sizeof(parser_flags[0]); ++i) {
        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));

        err = cbor_parser_init(Data, Size, parser_flags[i], &parser, &it);
        if (err != CborNoError)
            continue;

        if (!can_use_value(&it))
            continue;

        (void)cbor_value_validate_basic(&it);
        (void)cbor_value_reparse(&it);

        {
            size_t j;
            for (j = 0; j < sizeof(validate_flags) / sizeof(validate_flags[0]); ++j)
                (void)cbor_value_validate(&it, validate_flags[j]);
        }

        exercise_value(&it, 0);

        if (cbor_value_is_container(&it)) {
            CborValue recursed;
            err = cbor_value_enter_container(&it, &recursed);
            if (err == CborNoError) {
                if (cbor_value_at_end(&recursed)) {
                    (void)cbor_value_leave_container(&it, &recursed);
                } else {
                    CborValue first = recursed;
                    (void)cbor_value_validate_basic(&first);
                    (void)cbor_value_validate(&first, 0);
                    (void)cbor_value_reparse(&first);
                    exercise_value(&first, 1);
                }
            }
        }

        {
            CborValue tmp = it;
            (void)_cbor_value_finish_string_iteration(&tmp);
        }
    }

    return 0;
}