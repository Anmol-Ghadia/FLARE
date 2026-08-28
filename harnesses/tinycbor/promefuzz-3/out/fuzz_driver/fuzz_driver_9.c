// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_byte_string_chunk at cbor.h:555:27 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_get_text_string_chunk at cbor.h:549:27 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError || !cbor_value_is_valid(&it)) {
        return 0;
    }

    for (int i = 0; i < 8; ++i) {
        if (!cbor_value_is_valid(&it))
            break;

        CborType type = cbor_value_get_type(&it);
        if (type == CborInvalidType)
            break;

        if (type == CborByteStringType) {
            const uint8_t *buf = NULL;
            size_t len = 0;
            CborValue next = it;
            (void)cbor_value_get_byte_string_chunk(&it, &buf, &len, &next);

            if (!cbor_value_is_valid(&next) || cbor_value_get_type(&next) == CborInvalidType)
                break;

            if (cbor_value_get_type(&next) == CborTextStringType) {
                const char *tbuf = NULL;
                size_t tlen = 0;
                CborValue next2 = next;
                (void)cbor_value_get_text_string_chunk(&next, &tbuf, &tlen, &next2);

                if (!cbor_value_is_valid(&next2) || cbor_value_get_type(&next2) == CborInvalidType)
                    break;

                it = next2;
                continue;
            }

            it = next;
            continue;
        } else if (type == CborTextStringType) {
            const char *tbuf = NULL;
            size_t tlen = 0;
            CborValue next = it;
            (void)cbor_value_get_text_string_chunk(&it, &tbuf, &tlen, &next);

            if (!cbor_value_is_valid(&next) || cbor_value_get_type(&next) == CborInvalidType)
                break;

            it = next;
            continue;
        } else {
            CborValue next = it;

            if (!cbor_value_is_valid(&next) || cbor_value_get_type(&next) == CborInvalidType)
                break;

            err = cbor_value_advance(&next);
            if (err != CborNoError)
                break;

            if (!cbor_value_is_valid(&next) || cbor_value_get_type(&next) == CborInvalidType)
                break;

            it = next;
        }
    }

    return 0;
}