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
#include "cborjson.h"

static CborError fuzz_stream_writer(void *token, const char *data, size_t len)
{
    FILE *out = (FILE *)token;
    if (out == NULL)
        return CborErrorIO;

    if (len == 0)
        return CborNoError;

    if (fwrite(data, 1, len, out) != len)
        return CborErrorIO;

    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *out = fopen("./dummy_file", "wb+");
    if (out == NULL)
        return 0;

    if (Size > 0) {
        fwrite(Data, 1, Size, out);
        fflush(out);
        rewind(out);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fclose(out);
        return 0;
    }

    {
        CborValue v = it;
        (void)cbor_value_to_pretty(out, &v);
        rewind(out);
    }

    {
        CborValue v = it;
        int flags = 0;
#ifdef CborConvertAddMetadata
        flags ^= CborConvertAddMetadata;
#endif
#ifdef CborConvertTagsToObjects
        flags ^= CborConvertTagsToObjects;
#endif
#ifdef CborConvertStringifyMapKeys
        flags ^= CborConvertStringifyMapKeys;
#endif
#ifdef CborPrettyIndicateIndeterminateLength
        flags ^= CborPrettyIndicateIndeterminateLength;
#endif
#ifdef CborPrettyShowStringFragments
        flags ^= CborPrettyShowStringFragments;
#endif
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &v, flags);
        rewind(out);
    }

    {
        CborValue v = it;
        (void)cbor_value_to_pretty_advance(out, &v);
        rewind(out);
    }

    {
        CborValue v = it;
        int flags = 0;
#ifdef CborPrettyIndicateIndeterminateLength
        flags ^= CborPrettyIndicateIndeterminateLength;
#endif
#ifdef CborPrettyShowStringFragments
        flags ^= CborPrettyShowStringFragments;
#endif
        (void)cbor_value_to_pretty_advance_flags(out, &v, flags);
        rewind(out);
    }

    {
        CborValue v = it;
        int flags = 0;
#ifdef CborConvertAddMetadata
        flags ^= CborConvertAddMetadata;
#endif
#ifdef CborConvertTagsToObjects
        flags ^= CborConvertTagsToObjects;
#endif
#ifdef CborConvertStringifyMapKeys
        flags ^= CborConvertStringifyMapKeys;
#endif
        (void)cbor_value_to_json_advance(out, &v, flags);
        rewind(out);
    }

    {
        CborValue v = it;
        unsigned int steps = 0;
        while (steps < 16) {
            if (v.type == CborInvalidType)
                break;
            err = cbor_value_advance(&v);
            if (err != CborNoError)
                break;
            steps++;
        }
    }

    {
        CborValue v = it;
        unsigned int steps = 0;
        while (steps < 8) {
            if (v.type == CborInvalidType)
                break;
            (void)cbor_value_to_pretty_advance(out, &v);
            rewind(out);
            steps++;
        }
    }

    {
        CborValue v = it;
        unsigned int steps = 0;
        while (steps < 8) {
            if (v.type == CborInvalidType)
                break;
            (void)cbor_value_to_json_advance(out, &v, 0);
            rewind(out);
            steps++;
        }
    }

    fclose(out);
    return 0;
}