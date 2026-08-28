#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cbor.h"
#include "cborjson.h"

static uint32_t read_u32(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < n && i < 4; ++i)
        v = (v << 8) | p[i];
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    if (Data == NULL || Size == 0)
        return 0;

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError)
        return 0;

    {
        uint32_t flags1 = 0;
        uint32_t flags2 = CborValidateCompleteData;
        if (Size >= 4)
            flags1 = read_u32(Data, 4);

        (void)cbor_value_validate(&it, flags1);
        (void)cbor_value_validate(&it, flags2);
    }

    {
        bool result = false;
        static const char *tests[] = {
            "",
            "a",
            "null",
            "true",
            "false",
            "0",
            "{}",
            "[]",
            "dummy_file"
        };
        size_t i;
        for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
            (void)cbor_value_text_string_equals(&it, tests[i], &result);
    }

    {
        CborValue tmp = it;
        (void)cbor_value_reparse(&tmp);
    }

    {
        CborValue tmp = it;
        (void)_cbor_value_finish_string_iteration(&tmp);
    }

    {
        FILE *f = fopen("./dummy_file", "wb+");
        if (f) {
            if (Size > 0)
                (void)fwrite(Data, 1, Size, f);
            (void)fflush(f);
            (void)rewind(f);

            {
                CborValue tmp = it;
                (void)cbor_value_to_json_advance(f, &tmp, 0);
            }

            (void)rewind(f);
            {
                CborValue tmp = it;
                int json_flags = 0;
#ifdef CborConvertAddMetadata
                json_flags |= CborConvertAddMetadata;
#endif
#ifdef CborConvertTagsToObjects
                json_flags |= CborConvertTagsToObjects;
#endif
#ifdef CborConvertStringifyMapKeys
                json_flags |= CborConvertStringifyMapKeys;
#endif
#ifdef CborConvertByteStringsToBase64Url
                json_flags |= CborConvertByteStringsToBase64Url;
#endif
#ifdef CborConvertRequireMapStringKeys
                json_flags |= CborConvertRequireMapStringKeys;
#endif
                (void)cbor_value_to_json_advance(f, &tmp, json_flags);
            }

            (void)rewind(f);
            (void)cbor_value_to_pretty(f, &it);

            fclose(f);
        }
    }

    return 0;
}