#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void exercise_string_apis(CborValue *it) {
    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_is_byte_string(it) || cbor_value_is_text_string(it)) {
        size_t len = 0;
        (void)cbor_value_get_string_length(it, &len);

        CborValue iter = *it;
        if (cbor_value_is_valid(&iter)) {
            (void)cbor_value_begin_string_iteration(&iter);
            (void)cbor_value_string_iteration_at_end(&iter);
        }
    }
}

static void walk_one_level(CborValue *it) {
    if (!cbor_value_is_valid(it))
        return;

    exercise_string_apis(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        memset(&recursed, 0, sizeof(recursed));
        recursed.type = CborInvalidType;

        if (cbor_value_enter_container(it, &recursed) == CborNoError) {
            if (cbor_value_is_valid(&recursed))
                exercise_string_apis(&recursed);

            while (cbor_value_is_valid(&recursed) && !cbor_value_at_end(&recursed)) {
                if (cbor_value_advance(&recursed) != CborNoError)
                    break;
            }

            if (cbor_value_is_valid(&recursed) && cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL)
        return 0;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        CborValue fake;
        memset(&fake, 0, sizeof(fake));
        fake.type = (Size > 0) ? Data[0] : CborInvalidType;
        fake.remaining = (uint32_t)Size;
        fake.extra = (uint16_t)(Size & 0xffff);

        (void)cbor_value_is_valid(&fake);
        (void)cbor_value_is_byte_string(&fake);
        (void)cbor_value_is_text_string(&fake);
        return 0;
    }

    walk_one_level(&it);

    {
        CborValue copy = it;
        if (cbor_value_is_valid(&copy)) {
            (void)cbor_value_is_valid(&copy);
            (void)cbor_value_is_byte_string(&copy);
            (void)cbor_value_is_text_string(&copy);
            if (cbor_value_is_byte_string(&copy) || cbor_value_is_text_string(&copy)) {
                size_t len = 0;
                (void)cbor_value_get_string_length(&copy, &len);
                (void)cbor_value_begin_string_iteration(&copy);
                (void)cbor_value_string_iteration_at_end(&copy);
            }
        }
    }

    return 0;
}