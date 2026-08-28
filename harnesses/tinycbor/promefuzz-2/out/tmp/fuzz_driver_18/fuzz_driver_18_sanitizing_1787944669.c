#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cbor.h"

static void fuzz_one_value(FILE *out, CborValue *it) {
    CborValue copy = *it;
    (void)cbor_value_to_pretty(out, &copy);

    copy = *it;
    (void)cbor_value_reparse(&copy);

    copy = *it;
    (void)cbor_value_skip_tag(&copy);
    (void)cbor_value_to_pretty(out, &copy);

    if (cbor_value_is_text_string(it) || cbor_value_is_byte_string(it)) {
        CborValue s = *it;
        (void)_cbor_value_begin_string_iteration(&s);
        (void)cbor_value_reparse(&s);
        (void)_cbor_value_finish_string_iteration(&s);

#ifndef NDEBUG
        if ((s.flags & CborIteratorFlag_IteratingStringChunks) == 0) {
            CborValue s2 = *it;
            (void)cbor_value_begin_string_iteration(&s2);
            (void)cbor_value_reparse(&s2);
            (void)_cbor_value_finish_string_iteration(&s2);
        }
#endif
    }
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

    fuzz_one_value(out, &it);

    {
        CborValue cur = it;
        for (int depth = 0; depth < 8; ++depth) {
            fuzz_one_value(out, &cur);
            if (cbor_value_at_end(&cur))
                break;
            if (cbor_value_advance(&cur) != CborNoError)
                break;
        }
    }

    {
        CborValue cur = it;
        (void)cbor_value_skip_tag(&cur);
        fuzz_one_value(out, &cur);
    }

    fclose(out);
    return 0;
}