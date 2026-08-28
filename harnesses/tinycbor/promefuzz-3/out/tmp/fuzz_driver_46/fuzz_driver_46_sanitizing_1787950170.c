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

static void consume_byte_string_apis(CborValue *it) {
    if (!it || !cbor_value_is_valid(it))
        return;

    if (!cbor_value_is_byte_string(it))
        return;

    size_t len = 0;
    CborError err = cbor_value_get_string_length(it, &len);

    {
        uint8_t stackbuf[256];
        size_t buflen = sizeof(stackbuf);
        CborValue next;
        (void)cbor_value_copy_byte_string(it, stackbuf, &buflen, &next);
    }

    {
        size_t buflen = 0;
        CborValue next;
        (void)cbor_value_copy_byte_string(it, NULL, &buflen, &next);
    }

    if (err == CborNoError) {
        size_t want = len;
        uint8_t *heapbuf = NULL;
        CborValue next;

        if (want > 0 && want < (1U << 20))
            heapbuf = (uint8_t *)malloc(want);
        else if (want == 0)
            heapbuf = (uint8_t *)malloc(1);

        if (heapbuf) {
            size_t buflen = want;
            (void)cbor_value_copy_byte_string(it, heapbuf, &buflen, &next);
            free(heapbuf);
        }

        if (want < 64) {
            uint8_t smallbuf[64];
            size_t buflen = want + 1;
            if (buflen > sizeof(smallbuf))
                buflen = sizeof(smallbuf);
            (void)cbor_value_copy_byte_string(it, smallbuf, &buflen, &next);
        }
    }

    {
        uint8_t *dup = NULL;
        size_t dup_len = 0;
        CborValue next;
        if (cbor_value_dup_byte_string(it, &dup, &dup_len, &next) == CborNoError) {
            free(dup);
        }
    }

    {
        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            int complete = 1;
            while (!cbor_value_string_iteration_at_end(&iter)) {
                const uint8_t *chunk = NULL;
                size_t chunk_len = 0;
                CborValue next;
                CborError e = cbor_value_get_byte_string_chunk(&iter, &chunk, &chunk_len, &next);
                if (e != CborNoError) {
                    complete = 0;
                    break;
                }
                iter = next;
            }
            if (complete)
                (void)cbor_value_finish_string_iteration(it, &iter);
        }
    }

    {
        CborValue iter = *it;
        if (_cbor_value_begin_string_iteration(&iter) == CborNoError) {
            int complete = 1;
            while (!cbor_value_string_iteration_at_end(&iter)) {
                const uint8_t *chunk = NULL;
                size_t chunk_len = 0;
                CborValue next;
                CborError e = cbor_value_get_byte_string_chunk(&iter, &chunk, &chunk_len, &next);
                if (e != CborNoError) {
                    complete = 0;
                    break;
                }
                iter = next;
            }
            if (complete)
                (void)cbor_value_finish_string_iteration(it, &iter);
        }
    }
}

static void walk_value(CborValue *it, int depth) {
    if (!it || !cbor_value_is_valid(it) || depth > 32)
        return;

    if (cbor_value_is_byte_string(it)) {
        consume_byte_string_apis(it);
        return;
    }

    if (cbor_value_is_array(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 64) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
        return;
    }

    if (cbor_value_is_map(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&rec) && count++ < 64) {
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
                if (cbor_value_at_end(&rec))
                    break;
                walk_value(&rec, depth + 1);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
        }
        return;
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
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    CborValue cur = it;
    int items = 0;
    while (!cbor_value_at_end(&cur) && items++ < 64) {
        walk_value(&cur, 0);
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}