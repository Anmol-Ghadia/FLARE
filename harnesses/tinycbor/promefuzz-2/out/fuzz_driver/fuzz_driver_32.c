// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_to_json_advance at cbortojson.c:786:11 in cborjson.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_to_pretty_advance at cborpretty_stdio.c:63:11 in cbor.h
// cbor_value_to_json_advance at cbortojson.c:786:11 in cborjson.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_to_pretty at cbor.h:712:27 in cbor.h
// cbor_value_to_pretty_stream at cborpretty.c:576:11 in cbor.h
// cbor_value_to_pretty_advance at cborpretty_stdio.c:63:11 in cbor.h
// cbor_value_to_pretty_advance_flags at cborpretty_stdio.c:83:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "cbor.h"
#include "cborjson.h"

static CborError fuzz_stream_writer(void *token, const char *fmt, ...)
{
    FILE *out = (FILE *)token;
    va_list ap;
    int ret;

    if (out == NULL)
        return CborErrorIO;

    va_start(ap, fmt);
    ret = vfprintf(out, fmt, ap);
    va_end(ap);

    if (ret < 0)
        return CborErrorIO;
    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;

    out = fopen("./dummy_file", "wb+");
    if (out == NULL)
        return 0;

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, out);
        (void)fflush(out);
        (void)rewind(out);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fclose(out);
        return 0;
    }

    {
        CborValue v = it;
        (void)cbor_value_to_pretty(out, &v);
        (void)fflush(out);
        (void)rewind(out);
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
        (void)fflush(out);
        (void)rewind(out);
    }

    {
        CborValue v = it;
        (void)cbor_value_to_pretty_advance(out, &v);
        (void)fflush(out);
        (void)rewind(out);
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
        (void)fflush(out);
        (void)rewind(out);
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
        (void)fflush(out);
        (void)rewind(out);
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
            if (cbor_value_to_pretty_advance(out, &v) != CborNoError)
                break;
            (void)fflush(out);
            (void)rewind(out);
            steps++;
        }
    }

    {
        CborValue v = it;
        unsigned int steps = 0;
        while (steps < 8) {
            if (v.type == CborInvalidType)
                break;
            if (cbor_value_to_json_advance(out, &v, 0) != CborNoError)
                break;
            (void)fflush(out);
            (void)rewind(out);
            steps++;
        }
    }

    fclose(out);
    return 0;
}