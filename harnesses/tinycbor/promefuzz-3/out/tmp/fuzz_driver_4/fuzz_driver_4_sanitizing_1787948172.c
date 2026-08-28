#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    CborType type = cbor_value_get_type(&it);
    (void)type;

    if (cbor_value_is_array(&it)) {
        bool known1 = cbor_value_is_length_known(&it);
        (void)known1;

        size_t array_len1 = 0;
        (void)cbor_value_get_array_length(&it, &array_len1);

        bool known2 = cbor_value_is_length_known(&it);
        (void)known2;

        size_t array_len2 = 0;
        (void)cbor_value_get_array_length(&it, &array_len2);
    }

    type = cbor_value_get_type(&it);
    (void)type;

    if (cbor_value_is_map(&it)) {
        bool known3 = cbor_value_is_length_known(&it);
        (void)known3;

        size_t map_len1 = 0;
        (void)cbor_value_get_map_length(&it, &map_len1);

        bool known4 = cbor_value_is_length_known(&it);
        (void)known4;

        size_t map_len2 = 0;
        (void)cbor_value_get_map_length(&it, &map_len2);
    }

    bool is_text = cbor_value_is_text_string(&it);
    bool is_bytes = cbor_value_is_byte_string(&it);
    (void)is_text;
    (void)is_bytes;

    if (is_text || is_bytes) {
        bool known5 = cbor_value_is_length_known(&it);
        (void)known5;

        size_t str_len1 = 0;
        (void)cbor_value_get_string_length(&it, &str_len1);

        size_t str_len2 = 0;
        (void)cbor_value_get_string_length(&it, &str_len2);
    }

    const uint8_t *next = cbor_value_get_next_byte(&it);
    (void)next;

    return 0;
}