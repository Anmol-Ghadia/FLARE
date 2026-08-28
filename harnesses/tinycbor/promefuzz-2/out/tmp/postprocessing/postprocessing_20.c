#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"
#include "cborjson.h"

static int consume_int(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < sizeof(v) ? *size : sizeof(v);
    if (n > 0) {
        memcpy(&v, *data, n);
        *data += n;
        *size -= n;
    }
    return v;
}

static int safe_pretty_flags(int raw) {
    (void)raw;
    return 0;
}

static int safe_json_flags(int raw) {
    return raw & 0x0f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;
    FILE *out;

    out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, out);
        (void)fflush(out);
        rewind(out);
    }

    err = cbor_parser_init(Data, Size, 0, &parser, &it);
    if (err != CborNoError) {
        fclose(out);
        return 0;
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty(out, &copy);
        (void)fflush(out);
    }

    {
        const uint8_t *p = Data;
        size_t n = Size;
        int flags = safe_json_flags(consume_int(&p, &n));
        CborValue copy = it;
        (void)cbor_value_to_json(out, &copy, flags);
        (void)fflush(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, safe_pretty_flags(0));
        (void)fflush(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);
        (void)fflush(out);
    }

    {
        const uint8_t *p = Data;
        size_t n = Size;
        int flags = safe_json_flags(consume_int(&p, &n));
        CborValue copy = it;
        (void)cbor_value_to_json_advance(out, &copy, flags);
        (void)fflush(out);
    }

    {
        CborValue iter = it;
        size_t steps = 0;

        while (!cbor_value_at_end(&iter) && steps < 32) {
            CborValue tmp;

            tmp = iter;
            (void)cbor_value_to_pretty(out, &tmp);

            tmp = iter;
            (void)cbor_value_to_json(out, &tmp, 0);

            tmp = iter;
            (void)cbor_value_to_pretty_advance(out, &tmp);

            tmp = iter;
            (void)cbor_value_to_json_advance(out, &tmp, 0);

            err = cbor_value_advance(&iter);
            if (err != CborNoError)
                break;

            steps++;
        }
        (void)fflush(out);
    }

    fclose(out);
    return 0;
}