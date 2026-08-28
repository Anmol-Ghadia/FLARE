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

static int fuzz_stream_writer(void *token, const char *fmt, ...)
{
    (void)fmt;
    if (token) {
        FILE *f = (FILE *)token;
        fputc('X', f);
    }
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size != 0)
        fwrite(Data, 1, Size, out);
    fflush(out);
    rewind(out);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fclose(out);
        return 0;
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty(out, &copy);
        rewind(out);
    }

    {
        CborValue copy = it;
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
#ifdef CborPrettyNumericEncodingIndicators
        flags ^= CborPrettyNumericEncodingIndicators;
#endif
#ifdef CborPrettyShowStringFragments
        flags ^= CborPrettyShowStringFragments;
#endif
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, flags);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);
        rewind(out);
    }

    {
        CborValue copy = it;
        int flags = 0;
#ifdef CborPrettyIndicateIndeterminateLength
        flags ^= CborPrettyIndicateIndeterminateLength;
#endif
#ifdef CborPrettyShowStringFragments
        flags ^= CborPrettyShowStringFragments;
#endif
#ifdef CborPrettyNumericEncodingIndicators
        flags ^= CborPrettyNumericEncodingIndicators;
#endif
        (void)cbor_value_to_pretty_advance_flags(out, &copy, flags);
        rewind(out);
    }

    {
        CborValue copy = it;
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
        (void)cbor_value_to_json_advance(out, &copy, flags);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)_cbor_value_finish_string_iteration(&copy);
    }

    fclose(out);
    return 0;
}