#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "cbor.h"

static void fuzz_string_apis(CborValue *it) {
    size_t length = 0;
    (void)cbor_value_get_string_length(it, &length);

    if (cbor_value_is_byte_string(it)) {
        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const uint8_t *buf = NULL;
                size_t len = 0;
                CborValue next;
                CborError err = cbor_value_get_byte_string_chunk(&iter, &buf, &len, &next);
                if (err != CborNoError)
                    break;
                iter = next;
                if (cbor_value_string_iteration_at_end(&iter))
                    break;
            }
            if (cbor_value_string_iteration_at_end(&iter))
                (void)cbor_value_finish_string_iteration(&iter);
        }

        iter = *it;
        if (_cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const uint8_t *buf = NULL;
                size_t len = 0;
                CborValue next;
                CborError err = cbor_value_get_byte_string_chunk(&iter, &buf, &len, &next);
                if (err != CborNoError)
                    break;
                iter = next;
                if (cbor_value_string_iteration_at_end(&iter))
                    break;
            }
            if (cbor_value_string_iteration_at_end(&iter))
                (void)_cbor_value_finish_string_iteration(&iter);
        }
    } else if (cbor_value_is_text_string(it)) {
        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const char *buf = NULL;
                size_t len = 0;
                CborValue next;
                CborError err = cbor_value_get_text_string_chunk(&iter, &buf, &len, &next);
                if (err != CborNoError)
                    break;
                iter = next;
                if (cbor_value_string_iteration_at_end(&iter))
                    break;
            }
            if (cbor_value_string_iteration_at_end(&iter))
                (void)cbor_value_finish_string_iteration(&iter);
        }

        iter = *it;
        if (_cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const char *buf = NULL;
                size_t len = 0;
                CborValue next;
                CborError err = cbor_value_get_text_string_chunk(&iter, &buf, &len, &next);
                if (err != CborNoError)
                    break;
                iter = next;
                if (cbor_value_string_iteration_at_end(&iter))
                    break;
            }
            if (cbor_value_string_iteration_at_end(&iter))
                (void)_cbor_value_finish_string_iteration(&iter);
        }
    }
}

static void fuzz_container(CborValue *it, int depth) {
    CborValue rec;
    if (cbor_value_enter_container(it, &rec) != CborNoError)
        return;

    for (int i = 0; i < 32 && !cbor_value_at_end(&rec); ++i) {
        CborValue current = rec;
        if (cbor_value_is_byte_string(&current) || cbor_value_is_text_string(&current))
            fuzz_string_apis(&current);
        else if (depth < 8 && cbor_value_is_container(&current))
            fuzz_container(&current, depth + 1);

        if (cbor_value_advance(&rec) != CborNoError)
            break;
    }

    if (cbor_value_at_end(&rec))
        (void)cbor_value_leave_container(it, &rec);
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
    if (cbor_parser_init(Data, Size, 0, &parser, &it) != CborNoError)
        return 0;

    if (cbor_value_is_byte_string(&it) || cbor_value_is_text_string(&it))
        fuzz_string_apis(&it);
    else if (cbor_value_is_container(&it))
        fuzz_container(&it, 0);

    return 0;
}