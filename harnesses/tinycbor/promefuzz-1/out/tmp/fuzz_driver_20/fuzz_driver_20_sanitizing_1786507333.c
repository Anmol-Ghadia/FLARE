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

static void fuzz_value(CborValue *it) {
    if (!it)
        return;

    if (cbor_value_is_byte_string(it)) {
        size_t length = 0;
        (void)cbor_value_get_string_length(it, &length);

        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const uint8_t *buf = NULL;
                size_t len = 0;
                CborValue next = iter;
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
                CborValue next = iter;
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
        size_t length = 0;
        (void)cbor_value_get_string_length(it, &length);

        CborValue iter = *it;
        if (cbor_value_begin_string_iteration(&iter) == CborNoError) {
            for (int i = 0; i < 64; ++i) {
                const void *buf = NULL;
                size_t len = 0;
                CborValue next = iter;
                CborError err = cbor_value_get_text_string_chunk(&iter, (const char **)&buf, &len, &next);
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
                const void *buf = NULL;
                size_t len = 0;
                CborValue next = iter;
                CborError err = cbor_value_get_text_string_chunk(&iter, (const char **)&buf, &len, &next);
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

    if (cbor_value_is_container(it)) {
        CborValue rec;
        if (cbor_value_enter_container(it, &rec) == CborNoError) {
            for (int i = 0; i < 32 && !cbor_value_at_end(&rec); ++i) {
                fuzz_value(&rec);
                if (cbor_value_advance(&rec) != CborNoError)
                    break;
            }
            (void)cbor_value_leave_container(it, &rec);
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
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    fuzz_value(&it);

    CborValue cur = it;
    for (int i = 0; i < 32 && !cbor_value_at_end(&cur); ++i) {
        fuzz_value(&cur);
        if (cbor_value_advance(&cur) != CborNoError)
            break;
    }

    return 0;
}