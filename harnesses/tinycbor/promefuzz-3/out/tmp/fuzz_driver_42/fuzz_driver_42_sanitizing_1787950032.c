#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "cbor.h"
#include "cborjson.h"

static CborError fuzz_stream_writer(void *token, const char *format, ...)
{
    FILE *fp = (FILE *)token;
    if (fp == NULL || format == NULL)
        return CborErrorIO;
    if (fputs(format, fp) < 0)
        return CborErrorIO;
    return CborNoError;
}

static int safe_flags_from_byte(uint8_t b)
{
    return (b & 1) ? CborConvertAddMetadata : 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;
    int flags = 0;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size > 0)
        flags = safe_flags_from_byte(Data[0]);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fclose(out);
        return 0;
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_json(out, &copy, flags);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty(out, &copy);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_json_advance(out, &copy, flags);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, 0);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, 0);
        rewind(out);
    }

    if (Size > 1) {
        int altflags = safe_flags_from_byte(Data[Size - 1]);
        CborValue copy1 = it;
        CborValue copy2 = it;
        (void)cbor_value_to_json(out, &copy1, altflags);
        rewind(out);
        (void)cbor_value_to_json_advance(out, &copy2, altflags);
        rewind(out);
    }

    fclose(out);
    return 0;
}