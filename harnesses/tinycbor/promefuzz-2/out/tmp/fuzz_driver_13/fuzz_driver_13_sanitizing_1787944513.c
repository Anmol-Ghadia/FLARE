#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void fuzz_one_value(const CborValue *it) {
    if (!it)
        return;

    size_t len = SIZE_MAX;
    const void *buf = NULL;
    CborValue tmp, next;
    CborError err;

    err = cbor_value_calculate_string_length(it, &len);
    (void)err;

    tmp = *it;
    err = _cbor_value_begin_string_iteration(&tmp);
    (void)err;

    len = SIZE_MAX;
    err = _cbor_value_get_string_chunk_size(&tmp, &len);
    (void)err;

    len = SIZE_MAX;
    buf = NULL;
    next = tmp;
    err = _cbor_value_get_string_chunk(&tmp, &buf, &len, &next);
    (void)err;

    len = SIZE_MAX;
    buf = NULL;
    err = _cbor_value_get_string_chunk(&tmp, &buf, &len, NULL);
    (void)err;

    len = SIZE_MAX;
    err = cbor_value_get_string_chunk_size(&tmp, &len);
    (void)err;

    err = _cbor_value_finish_string_iteration(&tmp);
    (void)err;
}

static void fuzz_container(CborValue *container, int depth) {
    CborValue recursed;
    int steps = 0;

    if (!container || depth > 3)
        return;

    if (cbor_value_enter_container(container, &recursed) != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed) && steps++ < 16) {
        fuzz_one_value(&recursed);

        if (cbor_value_is_container(&recursed))
            fuzz_container(&recursed, depth + 1);

        if (cbor_value_advance(&recursed) != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *fp;
    CborValue cur;
    int steps = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fuzz_one_value(&it);
        return 0;
    }

    fuzz_one_value(&it);

    cur = it;
    while (!cbor_value_at_end(&cur) && steps++ < 32) {
        fuzz_one_value(&cur);

        if (cbor_value_is_container(&cur))
            fuzz_container(&cur, 0);

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}