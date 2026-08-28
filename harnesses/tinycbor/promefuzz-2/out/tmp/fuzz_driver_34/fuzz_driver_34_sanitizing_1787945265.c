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

static void consume_string_ops(CborValue *it) {
    CborValue tmp = *it;
    CborError err;
    bool eq = false;
    void *buffer = NULL;
    size_t buflen = 0;
    CborValue next;

    err = _cbor_value_begin_string_iteration(&tmp);
    if (err != CborNoError)
        return;

    (void)cbor_value_text_string_equals(&tmp, "", &eq);
    (void)cbor_value_text_string_equals(&tmp, "a", &eq);
    (void)cbor_value_text_string_equals(&tmp, "dummy", &eq);

    err = _cbor_value_dup_string(&tmp, &buffer, &buflen, &next);
    if (err == CborNoError)
        free(buffer);

    (void)_cbor_value_finish_string_iteration(&tmp);
}

static void traverse_value(CborValue *it, int depth) {
    if (depth > 8)
        return;

    bool eq = false;
    (void)cbor_value_text_string_equals(it, "", &eq);
    (void)cbor_value_text_string_equals(it, "key", &eq);
    (void)cbor_value_text_string_equals(it, "value", &eq);

    if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) {
        consume_string_ops(it);
    }

    if (cbor_value_is_map(it)) {
        CborValue found;
        (void)cbor_value_map_find_value(it, "", &found);
        (void)cbor_value_map_find_value(it, "a", &found);
        (void)cbor_value_map_find_value(it, "key", &found);
        (void)cbor_value_map_find_value(it, "dummy", &found);
    }

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            while (!cbor_value_at_end(&recursed)) {
                traverse_value(&recursed, depth + 1);
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
            if (err == CborNoError && cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    traverse_value(&it, 0);

    CborValue cursor = it;
    for (int i = 0; i < 16 && !cbor_value_at_end(&cursor); i++) {
        traverse_value(&cursor, 0);
        err = cbor_value_advance(&cursor);
        if (err != CborNoError)
            break;
    }

    return 0;
}