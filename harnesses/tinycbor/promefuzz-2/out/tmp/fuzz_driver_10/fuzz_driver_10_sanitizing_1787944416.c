#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void fuzz_one_value(CborValue *it) {
    size_t len = SIZE_MAX;
    size_t buflen;
    CborError err;
    CborValue next;
    const void *chunkptr = NULL;
    void *dupbuf = NULL;
    char *buf = NULL;

    err = cbor_value_calculate_string_length(it, &len);
    (void)err;

    err = _cbor_value_get_string_chunk_size(it, &len);
    (void)err;

    len = 0;
    err = _cbor_value_get_string_chunk(it, &chunkptr, &len, &next);
    (void)err;
    (void)chunkptr;

    buflen = 0;
    err = _cbor_value_copy_string(it, NULL, &buflen, &next);
    (void)err;

    buflen = len;
    if (buflen > 0 && buflen < (1U << 20)) {
        buf = (char *)malloc(buflen + 1);
        if (buf) {
            size_t tmp = buflen;
            err = _cbor_value_copy_string(it, buf, &tmp, &next);
            (void)err;
            buf[tmp < buflen + 1 ? tmp : buflen] = '\0';

            tmp = buflen;
            err = cbor_value_copy_text_string(it, buf, &tmp, &next);
            (void)err;

            if (buflen > 0) {
                size_t small = buflen / 2;
                err = _cbor_value_copy_string(it, buf, &small, &next);
                (void)err;

                small = buflen / 2;
                err = cbor_value_copy_text_string(it, buf, &small, &next);
                (void)err;
            }

            free(buf);
        }
    }

    buflen = 0;
    err = _cbor_value_dup_string(it, &dupbuf, &buflen, &next);
    (void)err;
    free(dupbuf);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    fuzz_one_value(&it);

    {
        CborValue cur = it;
        int steps = 0;
        while (steps++ < 32) {
            fuzz_one_value(&cur);
            err = cbor_value_advance(&cur);
            if (err != CborNoError)
                break;
        }
    }

    return 0;
}