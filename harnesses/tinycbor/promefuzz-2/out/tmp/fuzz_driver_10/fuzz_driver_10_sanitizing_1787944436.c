#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static int is_target_string_type(const CborValue *it) {
    return cbor_value_is_byte_string(it) || cbor_value_is_text_string(it);
}

static void fuzz_string_value(const CborValue *it) {
    size_t len = SIZE_MAX;
    size_t buflen = 0;
    size_t tmp;
    CborError err;
    CborValue next;
    const void *chunkptr = NULL;
    void *dupbuf = NULL;
    char *buf = NULL;
    uint8_t *rawbuf = NULL;

    if (!it || !is_target_string_type(it))
        return;

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

    if (buflen < (1U << 20)) {
        rawbuf = (uint8_t *)malloc(buflen > 0 ? buflen : 1);
        if (rawbuf) {
            tmp = buflen;
            err = _cbor_value_copy_string(it, rawbuf, &tmp, &next);
            (void)err;

            if (buflen > 0) {
                size_t small = buflen / 2;
                err = _cbor_value_copy_string(it, rawbuf, &small, &next);
                (void)err;
            }

            free(rawbuf);
        }
    }

    if (cbor_value_is_text_string(it) && buflen < (1U << 20)) {
        buf = (char *)malloc(buflen + 1);
        if (buf) {
            tmp = buflen;
            err = cbor_value_copy_text_string(it, buf, &tmp, &next);
            (void)err;
            if (tmp <= buflen)
                buf[tmp] = '\0';
            else
                buf[buflen] = '\0';

            if (buflen > 0) {
                size_t small = buflen / 2;
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

    fuzz_string_value(&it);

    return 0;
}