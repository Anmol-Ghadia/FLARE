// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_calculate_string_length at cborparser.c:954:11 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// _cbor_value_get_string_chunk_size at cborparser.c:1033:11 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// cbor_value_get_string_chunk_size at cbor.h:520:27 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static void fuzz_string_apis(const CborValue *it) {
    CborValue tmp, next;
    CborError err;
    size_t len = SIZE_MAX;
    const void *buf = NULL;

    if (!it)
        return;

    if (!cbor_value_is_byte_string(it) && !cbor_value_is_text_string(it))
        return;

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

    /* Do not call _cbor_value_finish_string_iteration() on fuzzed/truncated data:
       it may advance past malformed chunk state and trigger OOB reads internally. */
}

static void fuzz_value(CborValue *it, int depth);

static void fuzz_container(CborValue *container, int depth) {
    CborValue recursed;
    int steps = 0;

    if (!container || depth > 3)
        return;

    if (!cbor_value_is_container(container))
        return;

    if (cbor_value_enter_container(container, &recursed) != CborNoError)
        return;

    while (!cbor_value_at_end(&recursed) && steps++ < 16) {
        fuzz_value(&recursed, depth + 1);

        if (cbor_value_advance(&recursed) != CborNoError)
            break;
    }
}

static void fuzz_value(CborValue *it, int depth) {
    if (!it)
        return;

    fuzz_string_apis(it);

    if (depth <= 3 && cbor_value_is_container(it))
        fuzz_container(it, depth);
}

int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it, cur;
    CborError err;
    FILE *fp;
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
    if (err != CborNoError)
        return 0;

    cur = it;
    while (!cbor_value_at_end(&cur) && steps++ < 32) {
        fuzz_value(&cur, 0);

        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}