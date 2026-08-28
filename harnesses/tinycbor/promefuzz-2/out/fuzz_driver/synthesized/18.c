// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_to_pretty at cbor.h:712:27 in cbor.h
// cbor_value_is_byte_string at cbor.h:468:22 in cbor.h
// cbor_value_is_text_string at cbor.h:470:22 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_begin_string_iteration at cbor.h:533:27 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "cbor.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f)
        return;
    if (Size)
        (void)fwrite(Data, 1, Size, f);
    fclose(f);
}

static void fuzz_valid_value(FILE *out, CborValue *it) {
    CborValue copy;

    copy = *it;
    (void)cbor_value_reparse(&copy);

    copy = *it;
    (void)cbor_value_skip_tag(&copy);

    copy = *it;
    (void)cbor_value_to_pretty(out, &copy);

    if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) {
        copy = *it;
        (void)_cbor_value_begin_string_iteration(&copy);
        (void)cbor_value_reparse(&copy);
        (void)_cbor_value_finish_string_iteration(&copy);

#ifndef NDEBUG
        copy = *it;
        if (!(copy.flags & CborIteratorFlag_IteratingStringChunks)) {
            (void)cbor_value_begin_string_iteration(&copy);
            (void)cbor_value_reparse(&copy);
            (void)_cbor_value_finish_string_iteration(&copy);
        }
#endif
    }
}

int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;

    write_dummy_file(Data, Size);

    out = fopen("./dummy_file", "ab+");
    if (!out)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        fuzz_valid_value(out, &it);
    }

    {
        static const uint8_t seeds[][16] = {
            { 0x00 },
            { 0x01 },
            { 0x17 },
            { 0x18, 0x00 },
            { 0x20 },
            { 0x40 },
            { 0x60 },
            { 0x61, 'A' },
            { 0x41, 'B' },
            { 0x7f, 0xff },
            { 0x5f, 0xff },
            { 0xc0, 0x00 },
            { 0xc1, 0x61, 'x' },
            { 0xf4 },
            { 0xf5 },
            { 0xf6 }
        };
        size_t i;

        for (i = 0; i < sizeof(seeds) / sizeof(seeds[0]); ++i) {
            err = cbor_parser_init(seeds[i], sizeof(seeds[i]), 0, &parser, &it);
            if (err == CborNoError)
                fuzz_valid_value(out, &it);
        }
    }

    fclose(out);
    return 0;
}