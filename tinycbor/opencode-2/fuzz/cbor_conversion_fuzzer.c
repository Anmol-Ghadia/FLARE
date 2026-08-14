#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "cbor.h"
#include "cborjson.h"

static CborError discard_stream(void *token, const char *format, ...)
{
    (void)token;
    (void)format;
    return CborNoError;
}

static void exercise_json_conversion(const CborValue *value, int flags)
{
    FILE *stream = tmpfile();
    CborValue copy = *value;

    if (stream == NULL)
        return;

    (void)cbor_value_to_json_advance(stream, &copy, flags);
    fclose(stream);
}

static void exercise_pretty_conversion(const CborValue *value, int flags)
{
    FILE *stream = tmpfile();
    CborValue copy = *value;

    (void)cbor_value_to_pretty_stream(discard_stream, NULL, &copy, flags);

    if (stream == NULL)
        return;

    copy = *value;
    (void)cbor_value_to_pretty_advance_flags(stream, &copy, flags);
    fclose(stream);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    CborParser parser;
    CborValue value;

    (void)size;

    if (cbor_parser_init(data, size, 0, &parser, &value) != CborNoError)
        return 0;

    exercise_json_conversion(&value, CborConvertDefaultFlags);
    exercise_json_conversion(&value, CborConvertAddMetadata |
                                     CborConvertTagsToObjects |
                                     CborConvertByteStringsToBase64Url |
                                     CborConvertStringifyMapKeys);

    exercise_pretty_conversion(&value, CborPrettyDefaultFlags);
    exercise_pretty_conversion(&value, CborPrettyNumericEncodingIndicators |
                                       CborPrettyIndicateIndeterminateLength |
                                       CborPrettyIndicateOverlongNumbers |
                                       CborPrettyShowStringFragments);

    return 0;
}
