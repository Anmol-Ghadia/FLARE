#include "cbor.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

enum {
    kMaxTopLevelValues = 256
};

static const int kPrettyFlags[] = {
    CborPrettyDefaultFlags,
    CborPrettyNumericEncodingIndicators,
    CborPrettyShowStringFragments,
    CborPrettyNumericEncodingIndicators |
        CborPrettyIndicateIndeterminateLength |
        CborPrettyIndicateOverlongNumbers |
        CborPrettyShowStringFragments
};

typedef struct PrettySink {
    char scratch[1024];
    size_t total;
} PrettySink;

static CborError pretty_stream(void *token, const char *fmt, ...)
{
    PrettySink *sink = (PrettySink *)token;
    va_list ap;
    int written;

    va_start(ap, fmt);
    written = vsnprintf(sink->scratch, sizeof(sink->scratch), fmt, ap);
    va_end(ap);

    if (written > 0)
        sink->total += (size_t)written;
    return CborNoError;
}

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

        for (j = 0; j < sizeof(kPrettyFlags) / sizeof(kPrettyFlags[0]); ++j) {
            PrettySink sink = { {0}, 0 };
            copy = value;
            (void)cbor_value_to_pretty_stream(pretty_stream, &sink, &copy, kPrettyFlags[j]);
            if (j == 0)
                next = cbor_value_get_next_byte(&copy);
        }
        if (next <= cursor || (size_t)(next - cursor) > remaining)
            break;

        remaining -= (size_t)(next - cursor);
        cursor = next;
    }

    return 0;
}
