#include "cbor.h"
#include "cborjson.h"

#include <stdint.h>
#include <stdio.h>

enum {
    kMaxTopLevelValues = 256
};

static const int kJsonFlags[] = {
    CborConvertDefaultFlags,
    CborConvertAddMetadata,
    CborConvertTagsToObjects,
    CborConvertByteStringsToBase64Url,
    CborConvertStringifyMapKeys,
    CborConvertAddMetadata |
        CborConvertTagsToObjects |
        CborConvertByteStringsToBase64Url |
        CborConvertStringifyMapKeys
};

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const uint8_t *cursor = data;
    size_t remaining = size;
    size_t i;

    for (i = 0; i < kMaxTopLevelValues && remaining != 0; ++i) {
        CborParser parser;
        CborValue value;
        CborValue copy;
        const uint8_t *next = cursor;
        size_t j;

        if (cbor_parser_init(cursor, remaining, 0, &parser, &value) != CborNoError)
            break;

        for (j = 0; j < sizeof(kJsonFlags) / sizeof(kJsonFlags[0]); ++j) {
            FILE *out = tmpfile();
            if (!out)
                return 0;

            copy = value;
            (void)cbor_value_to_json_advance(out, &copy, kJsonFlags[j]);
            if (j == 0)
                next = cbor_value_get_next_byte(&copy);
            fclose(out);
        }
        if (next <= cursor || (size_t)(next - cursor) > remaining)
            break;

        remaining -= (size_t)(next - cursor);
        cursor = next;
    }

    return 0;
}
