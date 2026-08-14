#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError) {
        return 0;
    }

    (void)cbor_value_is_array(&it);
    (void)cbor_value_is_byte_string(&it);
    (void)cbor_value_is_text_string(&it);

    if (cbor_value_is_byte_string(&it) || cbor_value_is_text_string(&it)) {
        CborValue iter = it;
        err = cbor_value_begin_string_iteration(&iter);
        if (err == CborNoError) {
            (void)cbor_value_is_byte_string(&iter);

            if (cbor_value_is_byte_string(&iter)) {
                size_t buflen1 = 0;
                CborValue next1;
                memset(&next1, 0, sizeof(next1));

                err = cbor_value_copy_byte_string(&iter, NULL, &buflen1, &next1);
                if (err == CborErrorOutOfMemory || err == CborNoError || err == CborErrorUnknownLength ||
                    err == CborErrorDataTooLarge || err == CborErrorUnexpectedEOF ||
                    err == CborErrorAdvancePastEOF || err == CborErrorIllegalType) {
                    uint8_t *buf = NULL;
                    size_t buflen2 = buflen1;

                    if (buflen2 > 0 && buflen2 < (1U << 20)) {
                        buf = (uint8_t *)malloc(buflen2);
                    } else if (buflen2 == 0) {
                        buf = (uint8_t *)malloc(1);
                    }

                    if (buf != NULL) {
                        CborValue next2;
                        memset(&next2, 0, sizeof(next2));
                        (void)cbor_value_copy_byte_string(&iter, buf, &buflen2, &next2);
                        free(buf);
                    }
                }
            }

            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_type(&iter);

            if (cbor_value_is_byte_string(&iter) || cbor_value_is_text_string(&iter)) {
                size_t slen = 0;
                (void)cbor_value_get_string_length(&iter, &slen);
            }

            (void)cbor_value_at_end(&iter);
            (void)cbor_value_get_next_byte(&iter);
        }
    } else {
        (void)cbor_value_at_end(&it);
        (void)cbor_value_get_type(&it);
        (void)cbor_value_at_end(&it);
        (void)cbor_value_get_next_byte(&it);
    }

    return 0;
}