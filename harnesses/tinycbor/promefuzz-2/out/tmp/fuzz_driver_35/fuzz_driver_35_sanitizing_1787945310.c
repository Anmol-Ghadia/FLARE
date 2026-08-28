#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void traverse_value(CborValue *it, int depth) {
    if (!it || depth > 16)
        return;

    (void)cbor_value_reparse(it);
    (void)cbor_value_validate_basic(it);
    (void)cbor_value_validate(it, 0);
    (void)cbor_value_validate(it, CborValidateCompleteData);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            int guard = 0;
            while (!cbor_value_at_end(&recursed) && guard++ < 128) {
                traverse_value(&recursed, depth + 1);
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
            if (err == CborNoError && cbor_value_at_end(&recursed)) {
                (void)cbor_value_leave_container(it, &recursed);
            }
        }
    } else {
        (void)_cbor_value_finish_string_iteration(it);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    uint32_t flags_list[] = {
        0,
        CborValidateBasic,
        CborValidateCanonicalFormat,
        CborValidateStrictMode,
        CborValidateStrictMode | CborValidateCanonicalFormat
    };
    size_t i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    for (i = 0; i < sizeof(flags_list) / sizeof(flags_list[0]); ++i) {
        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));

        err = cbor_parser_init(Data, Size, flags_list[i], &parser, &it);
        if (err != CborNoError)
            continue;

        (void)cbor_value_validate_basic(&it);
        (void)cbor_value_validate(&it, 0);
        (void)cbor_value_validate(&it, CborValidateCompleteData);
        (void)cbor_value_validate(&it, flags_list[i]);
        (void)cbor_value_reparse(&it);

        {
            CborValue copy = it;
            int guard = 0;
            while (!cbor_value_at_end(&copy) && guard++ < 128) {
                traverse_value(&copy, 0);
                err = cbor_value_advance(&copy);
                if (err != CborNoError)
                    break;
                (void)cbor_value_reparse(&copy);
                (void)cbor_value_validate_basic(&copy);
            }
        }

        if (cbor_value_is_container(&it)) {
            CborValue recursed;
            err = cbor_value_enter_container(&it, &recursed);
            if (err == CborNoError) {
                int guard = 0;
                while (!cbor_value_at_end(&recursed) && guard++ < 128) {
                    (void)cbor_value_reparse(&recursed);
                    (void)cbor_value_validate_basic(&recursed);
                    (void)cbor_value_validate(&recursed, flags_list[i]);
                    err = cbor_value_advance(&recursed);
                    if (err != CborNoError)
                        break;
                }
                if (err == CborNoError && cbor_value_at_end(&recursed)) {
                    (void)cbor_value_leave_container(&it, &recursed);
                }
            }
        } else {
            CborValue tmp = it;
            (void)_cbor_value_finish_string_iteration(&tmp);
        }
    }

    return 0;
}