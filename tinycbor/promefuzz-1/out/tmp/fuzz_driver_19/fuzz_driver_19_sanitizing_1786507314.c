#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "cbor.h"
#include "cborjson.h"

static CborError fuzz_stream_writer(void *token, const char *format, ...)
{
    FILE *out = (FILE *)token;
    va_list ap;
    int ret;

    if (!out || !format)
        return CborErrorIO;

    va_start(ap, format);
    ret = vfprintf(out, format, ap);
    va_end(ap);

    return ret < 0 ? CborErrorIO : CborNoError;
}

static int get_json_flags(const uint8_t *Data, size_t Size, size_t offset)
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
    int json_flags1, json_flags2;

    if (!Data)
        return 0;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size > 0) {
        if (fwrite(Data, 1, Size, out) != Size) {
            fclose(out);
            return 0;
        }
    }
    fflush(out);
    rewind(out);

    json_flags1 = get_json_flags(Data, Size, 0);
    json_flags2 = get_json_flags(Data, Size, Size / 2);

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err == CborNoError) {
        copy = it;
        (void)cbor_value_to_pretty(out, &copy);

        copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);

        copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, CborPrettyDefaultFlags);

        copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, CborPrettyDefaultFlags);

        copy = it;
        (void)cbor_value_to_json(out, &copy, json_flags1);

        copy = it;
        (void)cbor_value_to_json_advance(out, &copy, json_flags2);

        if (!cbor_value_at_end(&it)) {
            CborValue seq = it;
            int steps = 0;
            while (!cbor_value_at_end(&seq) && steps < 8) {
                CborValue tmp = seq;
                (void)cbor_value_to_pretty(out, &tmp);
                (void)cbor_value_to_json(out, &tmp, json_flags1);

                err = cbor_value_to_pretty_advance(out, &seq);
                if (err != CborNoError)
                    break;
                ++steps;
            }
        }
    }

    err = cbor_parser_init(Data, Size, CborValidateBasic, &parser, &it);
    if (err == CborNoError) {
        copy = it;
        (void)cbor_value_to_pretty_stream(fuzz_stream_writer, out, &copy, CborPrettyDefaultFlags);

        copy = it;
        (void)cbor_value_to_json_advance(out, &copy, json_flags1);

        copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, CborPrettyDefaultFlags);
    }

    fclose(out);
    return 0;
}