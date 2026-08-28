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

static int consume_int(const uint8_t **data, size_t *size) {
    int v = 0;
    size_t n = *size < sizeof(int) ? *size : sizeof(int);
    if (n > 0) {
        memcpy(&v, *data, n);
        *data += n;
        *size -= n;
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    CborParser parser;
    CborValue it;
    CborError err;

    FILE *out = fopen("./dummy_file", "wb+");
    if (!out)
        return 0;

    if (Size > 0) {
        fwrite(Data, 1, Size, out);
        fflush(out);
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
        fflush(out);
    }

    {
        const uint8_t *p = Data;
        size_t n = Size;
        int flags = consume_int(&p, &n);

        CborValue copy = it;
        (void)cbor_value_to_json(out, &copy, flags);
        fflush(out);
    }

    {
        const uint8_t *p = Data;
        size_t n = Size;
        int flags = consume_int(&p, &n);

        CborValue copy = it;
        (void)cbor_value_to_pretty_advance_flags(out, &copy, flags);
        fflush(out);
    }

    {
        CborValue copy = it;
        (void)cbor_value_to_pretty_advance(out, &copy);
        fflush(out);
    }

    {
        const uint8_t *p = Data;
        size_t n = Size;
        int flags = consume_int(&p, &n);

        CborValue copy = it;
        (void)cbor_value_to_json_advance(out, &copy, flags);
        fflush(out);
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
        fflush(out);
    }

    fclose(out);
    return 0;
}