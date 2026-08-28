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
    FILE *fp = (FILE *)token;
    if (fp == NULL)
        return CborErrorIO;
    if (len == 0)
        return CborNoError;
    return fwrite(data, 1, len, fp) == len ? CborNoError : CborErrorIO;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;
    int flags;
    uint8_t selector = 0;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size > 0)
        selector = Data[0];

    flags = (int)selector;

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
        (void)cbor_value_to_pretty_advance_flags(out, &copy, flags);
        rewind(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, flags);
        rewind(out);
    }

    if (Size > 1) {
        int altflags1 = (int)((selector << 8) | Data[Size - 1]);
        CborValue copy = it;
        (void)cbor_value_to_json(out, &copy, altflags1);
        rewind(out);
    }

    if (Size > 2) {
        int altflags2 = (int)((Data[1] << 16) | (Data[2] << 8) | selector);
        CborValue copy1 = it;
        CborValue copy2 = it;
        (void)cbor_value_to_json_advance(out, &copy1, altflags2);
        rewind(out);
        (void)cbor_value_to_pretty_advance_flags(out, &copy2, altflags2);
        rewind(out);
    }

    fclose(out);
    return 0;
}