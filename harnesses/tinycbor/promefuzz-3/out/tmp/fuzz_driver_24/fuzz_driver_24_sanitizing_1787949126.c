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

static void write_dummy_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (size)
        fwrite(data, 1, size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    write_dummy_file(Data, Size);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        CborValue reparsed = it;
        (void)cbor_value_reparse(&reparsed);
    }

    {
        bool eq = false;
        (void)cbor_value_text_string_equals(&it, "", &eq);
        if (Size > 0) {
            size_t n = Size > 32 ? 32 : Size;
            char *cmp = (char *)malloc(n + 1);
            if (cmp) {
                memcpy(cmp, Data, n);
                cmp[n] = '\0';
                (void)cbor_value_text_string_equals(&it, cmp, &eq);
                free(cmp);
            }
        }
    }

    if (cbor_value_is_text_string(&it)) {
        char *buffer = NULL;
        size_t buflen = 0;
        CborValue next = it;
        (void)cbor_value_dup_text_string(&it, &buffer, &buflen, &next);
        free(buffer);

        {
            CborValue reparsed = it;
            (void)cbor_value_reparse(&reparsed);
        }

        if (cbor_value_is_length_known(&it)) {
            size_t len = 0;
            const void *ptr = NULL;
            CborValue tmp = it;
            if (cbor_value_get_text_string_chunk(&tmp, &ptr, &len, &tmp) == CborNoError) {
                (void)ptr;
            }
        } else {
            CborValue recursed;
            if (cbor_value_enter_container(&it, &recursed) == CborNoError) {
                for (int i = 0; i < 32; ++i) {
                    size_t chunk_len = 0;
                    CborError e = cbor_value_get_string_chunk_size(&recursed, &chunk_len);
                    if (e != CborNoError)
                        break;

                    {
                        const void *chunk_ptr = NULL;
                        CborValue next_chunk = recursed;
                        e = cbor_value_get_text_string_chunk(&recursed, &chunk_ptr, &chunk_len, &next_chunk);
                        if (e != CborNoError)
                            break;
                        recursed = next_chunk;
                    }
                }
                (void)_cbor_value_finish_string_iteration(&recursed);
            }
        }
    }

    if (cbor_value_is_map(&it)) {
        CborValue found;
        (void)cbor_value_map_find_value(&it, "", &found);

        if (Size > 0) {
            size_t n = Size > 32 ? 32 : Size;
            char *key = (char *)malloc(n + 1);
            if (key) {
                memcpy(key, Data, n);
                key[n] = '\0';
                (void)cbor_value_map_find_value(&it, key, &found);
                free(key);
            }
        }
    }

    {
        CborValue cursor = it;
        for (int depth = 0; depth < 16; ++depth) {
            (void)cbor_value_reparse(&cursor);

            if (cbor_value_is_map(&cursor)) {
                CborValue found;
                (void)cbor_value_map_find_value(&cursor, "a", &found);
                break;
            } else if (cbor_value_is_text_string(&cursor)) {
                bool eq = false;
                (void)cbor_value_text_string_equals(&cursor, "a", &eq);
                break;
            } else if (cbor_value_is_container(&cursor)) {
                CborValue child;
                if (cbor_value_enter_container(&cursor, &child) != CborNoError)
                    break;
                cursor = child;
            } else {
                break;
            }
        }
    }

    return 0;
}