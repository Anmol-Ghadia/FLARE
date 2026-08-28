// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// _cbor_value_dup_string at cborparser_dup_string.c:96:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_map at cbor.h:567:22 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_map_find_value at cborparser.c:1402:11 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cbor.h"

static bool safe_can_touch(const CborValue *it) {
    if (it == NULL || it->parser == NULL)
        return false;
    if (it->source.ptr == NULL)
        return false;
    return it->source.ptr < it->parser->source.end;
}

static void consume_string_ops(const CborValue *it) {
    CborValue tmp;
    CborError err;
    bool eq = false;
    void *buffer = NULL;
    size_t buflen = 0;
    CborValue next;

    if (!safe_can_touch(it))
        return;

    memcpy(&tmp, it, sizeof(tmp));

    err = _cbor_value_begin_string_iteration(&tmp);
    if (err != CborNoError)
        return;

    if (safe_can_touch(&tmp)) {
        (void)cbor_value_text_string_equals(&tmp, "", &eq);
        (void)cbor_value_text_string_equals(&tmp, "a", &eq);
        (void)cbor_value_text_string_equals(&tmp, "dummy", &eq);

        err = _cbor_value_dup_string(&tmp, &buffer, &buflen, &next);
        if (err == CborNoError)
            free(buffer);
    }

    if (safe_can_touch(&tmp))
        (void)_cbor_value_finish_string_iteration(&tmp);
}

static void traverse_value(CborValue *it, int depth) {
    if (depth > 8 || !safe_can_touch(it))
        return;

    bool eq = false;
    (void)cbor_value_text_string_equals(it, "", &eq);
    (void)cbor_value_text_string_equals(it, "key", &eq);
    (void)cbor_value_text_string_equals(it, "value", &eq);

    if ((cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) && safe_can_touch(it))
        consume_string_ops(it);

    if (cbor_value_is_map(it) && safe_can_touch(it)) {
        CborValue found;
        (void)cbor_value_map_find_value(it, "", &found);
        (void)cbor_value_map_find_value(it, "a", &found);
        (void)cbor_value_map_find_value(it, "key", &found);
        (void)cbor_value_map_find_value(it, "dummy", &found);
    }

    if (cbor_value_is_container(it) && safe_can_touch(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err != CborNoError || !safe_can_touch(&recursed))
            return;

        while (!cbor_value_at_end(&recursed) && safe_can_touch(&recursed)) {
            CborValue current;
            memcpy(&current, &recursed, sizeof(current));

            traverse_value(&current, depth + 1);

            err = cbor_value_advance(&recursed);
            if (err != CborNoError)
                return;
        }

        if (cbor_value_at_end(&recursed))
            (void)cbor_value_leave_container(it, &recursed);
    }
}

int LLVMFuzzerTestOneInput_34(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size != 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    if (safe_can_touch(&it))
        traverse_value(&it, 0);

    return 0;
}