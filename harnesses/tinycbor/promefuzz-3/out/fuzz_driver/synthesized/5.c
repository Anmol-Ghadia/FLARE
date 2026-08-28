// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_get_int64 at cbor.h:432:27 in cbor.h
// cbor_value_dup_byte_string at cbor.h:512:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_dup_text_string at cbor.h:506:27 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput_5(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        return 0;
    }

    bool at_end = cbor_value_at_end(&it);
    (void)at_end;

    CborType type = cbor_value_get_type(&it);
    (void)type;

    bool is_container = cbor_value_is_container(&it);
    (void)is_container;

    if (type == CborIntegerType) {
        int64_t v = 0;
        (void)cbor_value_get_int64(&it, &v);
    }

    if (type == CborByteStringType) {
        uint8_t *buffer = NULL;
        size_t buflen = 0;
        CborValue next = it;
        err = cbor_value_dup_byte_string(&it, &buffer, &buflen, &next);
        if (err == CborNoError) {
            bool next_at_end = cbor_value_at_end(&next);
            CborType next_type = cbor_value_get_type(&next);
            bool next_is_container = cbor_value_is_container(&next);
            (void)next_at_end;
            (void)next_type;
            (void)next_is_container;
        }
        free(buffer);
    }

    if (type == CborTextStringType) {
        char *buffer = NULL;
        size_t buflen = 0;
        CborValue next = it;
        err = cbor_value_dup_text_string(&it, &buffer, &buflen, &next);
        if (err == CborNoError) {
            bool next_at_end = cbor_value_at_end(&next);
            CborType next_type = cbor_value_get_type(&next);
            bool next_is_container = cbor_value_is_container(&next);
            (void)next_at_end;
            (void)next_type;
            (void)next_is_container;
        }
        free(buffer);
    }

    return 0;
}