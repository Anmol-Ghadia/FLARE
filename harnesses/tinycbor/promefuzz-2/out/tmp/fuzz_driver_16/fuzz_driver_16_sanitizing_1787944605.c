#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "cbor.h"

static void exercise_value(CborValue *it) {
    if (!cbor_value_is_valid(it))
        return;

    bool is_bs = cbor_value_is_byte_string(it);
    bool is_ts = cbor_value_is_text_string(it);

    if (is_bs || is_ts) {
        size_t len = 0;
        (void)cbor_value_get_string_length(it, &len);

        CborValue iter = *it;
        if (cbor_value_is_valid(&iter)) {
            (void)cbor_value_begin_string_iteration(&iter);
            (void)cbor_value_string_iteration_at_end(&iter);
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
        fake.type = (Size > 0) ? Data[0] : 0;
        fake.remaining = (uint32_t)Size;
        fake.extra = (uint16_t)(Size & 0xffff);

        (void)cbor_value_is_valid(&fake);
        (void)cbor_value_is_byte_string(&fake);
        (void)cbor_value_is_text_string(&fake);
        return 0;
    }

    exercise_value(&it);

    {
        CborValue copy1 = it;
        if (cbor_value_is_valid(&copy1)) {
            (void)cbor_value_is_byte_string(&copy1);
            (void)cbor_value_is_text_string(&copy1);
            if (cbor_value_is_byte_string(&copy1) || cbor_value_is_text_string(&copy1)) {
                size_t len = 0;
                (void)cbor_value_get_string_length(&copy1, &len);
                (void)cbor_value_begin_string_iteration(&copy1);
                (void)cbor_value_string_iteration_at_end(&copy1);
            }
        }
    }

    {
        CborValue next = it;
        if (cbor_value_is_valid(&next)) {
            (void)cbor_value_advance_fixed(&next);
            exercise_value(&next);
        }
    }

    {
        CborValue nested = it;
        CborValue recursed;
        if (cbor_value_is_valid(&nested) && cbor_value_is_container(&nested)) {
            if (cbor_value_enter_container(&nested, &recursed) == CborNoError) {
                exercise_value(&recursed);
                (void)cbor_value_leave_container(&nested, &recursed);
            }
        }
    }

    return 0;
}