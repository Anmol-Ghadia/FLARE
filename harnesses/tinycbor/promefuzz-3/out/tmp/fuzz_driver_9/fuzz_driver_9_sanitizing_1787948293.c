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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    /* Optional file setup as requested when file interaction is needed. */
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    /* Walk a few items to explore more states while preserving required call order. */
    for (int i = 0; i < 8; ++i) {
        CborType type = cbor_value_get_type(&it);

        if (type == CborByteStringType) {
            const uint8_t *buf = NULL;
            size_t len = 0;
            CborValue next = it;
            (void)cbor_value_get_byte_string_chunk(&it, &buf, &len, &next);

            /* Required order includes text string chunk after byte string chunk. */
            if (cbor_value_get_type(&next) == CborTextStringType) {
                const char *tbuf = NULL;
                size_t tlen = 0;
                CborValue next2 = next;
                (void)cbor_value_get_text_string_chunk(&next, &tbuf, &tlen, &next2);
                it = next2;
                continue;
            }

            if (cbor_value_is_valid(&next)) {
                it = next;
                continue;
            }
            break;
        } else if (type == CborTextStringType) {
            /* Still respect the requested order by calling get_type first,
               and only calling the string-chunk APIs when types match. */
            const char *tbuf = NULL;
            size_t tlen = 0;
            CborValue next = it;
            (void)cbor_value_get_text_string_chunk(&it, &tbuf, &tlen, &next);

            if (cbor_value_is_valid(&next)) {
                it = next;
                continue;
            }
            break;
        } else {
            CborValue next = it;
            err = cbor_value_advance(&next);
            if (err != CborNoError)
                break;
            it = next;
        }
    }

    return 0;
}