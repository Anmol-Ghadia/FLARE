#include "cbor.h"
#include "cborjson.h"

#include <stdint.h>
#include <stdio.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    CborError err;
    CborParser parser;
    CborValue value;
    int pretty_flags = CborPrettyDefaultFlags;
    int json_flags = CborConvertDefaultFlags;

    if (size > 0) {
        if (data[0] & 0x01)
            pretty_flags |= CborPrettyNumericEncodingIndicators;
        if (data[0] & 0x02)
            pretty_flags |= CborPrettyIndicateOverlongNumbers;
        if (data[0] & 0x04)
            pretty_flags |= CborPrettyShowStringFragments;
    }

    if (size > 1) {
        if (data[1] & 0x01)
            json_flags |= CborConvertAddMetadata;
        if (data[1] & 0x02)
            json_flags |= CborConvertTagsToObjects;
        if (data[1] & 0x04)
            json_flags |= CborConvertByteStringsToBase64Url;
        if (data[1] & 0x08)
            json_flags |= CborConvertStringifyMapKeys;
    }

    err = cbor_parser_init(data, size, 0, &parser, &value);
    if (err != CborNoError) {
        (void)cbor_error_string(err);
        return 0;
    }

    (void)cbor_value_validate(&value, CborValidateCompleteData);

    {
        CborValue pretty_value = value;
        FILE *pretty_output = tmpfile();
        if (pretty_output != NULL) {
            err = cbor_value_to_pretty_advance_flags(pretty_output, &pretty_value, pretty_flags);
            (void)cbor_error_string(err);
            fclose(pretty_output);
        }
    }

    {
        CborValue json_value = value;
        FILE *json_output = tmpfile();
        if (json_output != NULL) {
            err = cbor_value_to_json_advance(json_output, &json_value, json_flags);
            (void)cbor_error_string(err);
            fclose(json_output);
        }
    }

    return 0;
}
