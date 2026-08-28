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
#include <stdio.h>

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);

    if (err == CborNoError) {
        (void)cbor_value_is_boolean(&it);
        (void)cbor_value_is_byte_string(&it);
        (void)cbor_value_is_text_string(&it);
        (void)cbor_value_string_iteration_at_end(&it);
        (void)cbor_value_is_null(&it);

        if (cbor_value_is_text_string(&it)) {
            const char *buffer = NULL;
            size_t len = 0;
            CborValue next = it;

            for (int i = 0; i < 8; ++i) {
                (void)cbor_value_string_iteration_at_end(&next);
                err = cbor_value_get_text_string_chunk(&next, &buffer, &len, &next);
                if (err != CborNoError)
                    break;
                if (cbor_value_string_iteration_at_end(&next))
                    break;
            }
        }

        {
            CborValue cur = it;
            for (int i = 0; i < 4; ++i) {
                (void)cbor_value_is_boolean(&cur);
                (void)cbor_value_is_byte_string(&cur);
                (void)cbor_value_is_text_string(&cur);
                (void)cbor_value_string_iteration_at_end(&cur);
                (void)cbor_value_is_null(&cur);

                CborValue next = cur;
                if (cbor_value_advance(&next) != CborNoError)
                    break;
                cur = next;
            }
        }
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0)
                (void)fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    {
        CborValue manual;
        memset(&manual, 0, sizeof(manual));
        manual.type = (Size > 0) ? Data[0] : 0;
        (void)cbor_value_is_boolean(&manual);
        (void)cbor_value_is_byte_string(&manual);
        (void)cbor_value_is_text_string(&manual);
        (void)cbor_value_string_iteration_at_end(&manual);
        (void)cbor_value_is_null(&manual);

        manual.type = CborBooleanType;
        (void)cbor_value_is_boolean(&manual);

        manual.type = CborByteStringType;
        (void)cbor_value_is_byte_string(&manual);

        manual.type = CborTextStringType;
        (void)cbor_value_is_text_string(&manual);

        manual.type = CborNullType;
        (void)cbor_value_is_null(&manual);
    }

    return 0;
}