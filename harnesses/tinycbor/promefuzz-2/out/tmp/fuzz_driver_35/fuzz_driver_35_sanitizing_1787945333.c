#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static int is_probably_valid_iterator(const CborValue *it) {
    return it && it->type != CborInvalidType;
}

static void exercise_current_value(CborValue *it, int depth) {
    if (!is_probably_valid_iterator(it) || depth > 16)
        return;

    (void)cbor_value_reparse(it);

    if (!is_probably_valid_iterator(it))
        return;

    (void)cbor_value_validate_basic(it);
    (void)cbor_value_validate(it, 0);
    (void)cbor_value_validate(it, CborValidateCompleteData);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError)
            return;

        if (!is_probably_valid_iterator(&recursed)) {
            if (cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
            return;
        }

        {
            int guard = 0;
            while (!cbor_value_at_end(&recursed) && guard++ < 128) {
                if (!is_probably_valid_iterator(&recursed))
                    break;

                exercise_current_value(&recursed, depth + 1);

                if (!is_probably_valid_iterator(&recursed))
                    break;

                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
        }

        if (err == CborNoError && cbor_value_at_end(&recursed))
            (void)cbor_value_leave_container(it, &recursed);
    } else {
        CborValue tmp = *it;
        if (is_probably_valid_iterator(&tmp))
            (void)_cbor_value_finish_string_iteration(&tmp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    uint32_t validate_flags[] = {
        0,
        CborValidateBasic,
        CborValidateCanonicalFormat,
        CborValidateStrictMode,
        CborValidateStrictMode | CborValidateCanonicalFormat,
        CborValidateCompleteData,
        CborValidateCompleteData | CborValidateStrictMode
    };
    size_t i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (!is_probably_valid_iterator(&it))
        return 0;

    (void)cbor_value_reparse(&it);
    if (!is_probably_valid_iterator(&it))
        return 0;

    (void)cbor_value_validate_basic(&it);

    for (i = 0; i < sizeof(validate_flags) / sizeof(validate_flags[0]); ++i) {
        if (!is_probably_valid_iterator(&it))
            break;
        (void)cbor_value_validate(&it, validate_flags[i]);
    }

    {
        CborValue copy = it;
        int guard = 0;

        while (!cbor_value_at_end(&copy) && guard++ < 128) {
            if (!is_probably_valid_iterator(&copy))
                break;

            exercise_current_value(&copy, 0);

            if (!is_probably_valid_iterator(&copy))
                break;

            err = cbor_value_advance(&copy);
            if (err != CborNoError)
                break;

            if (cbor_value_at_end(&copy))
                break;

            if (!is_probably_valid_iterator(&copy))
                break;

            (void)cbor_value_reparse(&copy);
        }
    }

    if (!is_probably_valid_iterator(&it))
        return 0;

    if (cbor_value_is_container(&it)) {
        CborValue recursed;
        err = cbor_value_enter_container(&it, &recursed);
        if (err == CborNoError) {
            int guard = 0;

            while (!cbor_value_at_end(&recursed) && guard++ < 128) {
                if (!is_probably_valid_iterator(&recursed))
                    break;

                (void)cbor_value_reparse(&recursed);
                if (!is_probably_valid_iterator(&recursed))
                    break;

                (void)cbor_value_validate_basic(&recursed);
                (void)cbor_value_validate(&recursed, 0);

                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }

            if (err == CborNoError && cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(&it, &recursed);
        }
    } else {
        CborValue tmp = it;
        if (is_probably_valid_iterator(&tmp))
            (void)_cbor_value_finish_string_iteration(&tmp);
    }

    return 0;
}