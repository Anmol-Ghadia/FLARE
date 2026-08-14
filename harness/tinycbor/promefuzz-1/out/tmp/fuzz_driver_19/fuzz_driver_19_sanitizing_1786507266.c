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

#ifndef CBOR_PARSER_MAX_RECURSIONS
#define CBOR_PARSER_MAX_RECURSIONS 1024
#endif

static CborError fuzz_stream_writer(void *token, const char *buffer, size_t len)
{
    FILE *out = (FILE *)token;
    if (!out)
        return CborErrorIO;

    if (len == 0)
        return CborNoError;

    return fwrite(buffer, 1, len, out) == len ? CborNoError : CborErrorIO;
}

static int get_flags_from_data(const uint8_t *Data, size_t Size, size_t offset)
{
    int flags = 0;
    size_t i;
    size_t end = offset + sizeof(int);
    if (end > Size)
        end = Size;

    for (i = offset; i < end; ++i)
        flags = (flags << 8) ^ Data[i];

    return flags;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborParser parser;
    CborValue it;
    CborValue copy;
    CborError err;
    FILE *out;
    int flags1, flags2, flags3;

    if (!Data || Size == 0)
        return 0;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (fwrite(Data, 1, Size, out) != Size) {
        fclose(out);
        return 0;
    }
    fflush(out);
    rewind(out);

    flags1 = get_flags_from_data(Data, Size, 0);
    flags2 = get_flags_from_data(Data, Size, Size / 3);
    flags3 = get_flags_from_data(Data, Size, (Size * 2) / 3);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        copy = it;
        (void)cbor_value_to_pretty(out, &copy);

        copy = it;
        (void)cbor_value_to_json(out, &copy, flags1);

        copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);

        copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, flags2);

        copy = it;
        (void)cbor_value_to_json_advance(out, &copy, flags3);

        copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, flags1);

        if (!cbor_value_at_end(&it)) {
            CborValue seq = it;
            int steps = 0;
            while (!cbor_value_at_end(&seq) && steps < 8) {
                CborValue tmp = seq;
                (void)cbor_value_to_pretty(out, &tmp);
                (void)cbor_value_to_json(out, &tmp, flags2);

                err = cbor_value_to_pretty_advance_flags(out, &seq, flags3);
                if (err != CborNoError)
                    break;
                ++steps;
            }
        }
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, flags2);

        copy = it;
        (void)cbor_value_to_json_advance(out, &copy, flags1);

        copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);
    }

    fclose(out);
    return 0;
}