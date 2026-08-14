// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_validate at cborvalidation.c:640:11 in cbor.h
// cbor_value_validate at cborvalidation.c:640:11 in cbor.h
// cbor_value_validate at cborvalidation.c:640:11 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_is_length_known at cbor.h:453:22 in cbor.h
// cbor_value_get_array_length at cbor.h:570:27 in cbor.h
// cbor_value_to_pretty at cbor.h:712:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cbor.h"

static uint32_t read_u32(const uint8_t *data, size_t size, size_t *offset) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < size; ++i, ++(*offset)) {
        v = (v << 8) | data[*offset];
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    size_t offset = 0;
    uint32_t flags = 0;
    FILE *out;
    CborType type;

    if (Size == 0)
        return 0;

    flags = read_u32(Data, Size, &offset);

    err = cbor_parser_init(Data + offset, Size - offset, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    (void)cbor_value_validate_basic(&it);
    (void)cbor_value_validate(&it, 0);
    (void)cbor_value_validate(&it, flags);
    (void)cbor_value_validate(&it, flags | CborValidateCompleteData);

    type = cbor_value_get_type(&it);
    (void)type;
    (void)cbor_value_is_length_known(&it);

    if (type == CborArrayType) {
        size_t length = 0;
        (void)cbor_value_get_array_length(&it, &length);
    }

    out = fopen("./dummy_file", "wb+");
    if (out != NULL) {
        (void)fwrite(Data, 1, Size, out);
        (void)fflush(out);
        rewind(out);
        (void)cbor_value_to_pretty(out, &it);
        fclose(out);
    }

    return 0;
}