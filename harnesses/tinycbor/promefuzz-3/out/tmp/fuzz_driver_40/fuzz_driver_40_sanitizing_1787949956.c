#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
    uint8_t scratch[4096];
} FuzzReader;

static bool fuzz_can_read_bytes(void *token, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    if (!r)
        return false;
    return r->pos <= r->size && len <= (r->size - r->pos);
}

static void *fuzz_read_bytes(void *token, void *dst, size_t offset, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    if (!r || offset > r->size || len > r->size - offset)
        return NULL;

    if (dst) {
        memcpy(dst, r->data + offset, len);
        return dst;
    }

    if (len <= sizeof(r->scratch)) {
        memcpy(r->scratch, r->data + offset, len);
        return r->scratch;
    }

    return NULL;
}

static void fuzz_advance_bytes(void *token, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    if (!r)
        return;
    if (len > r->size - r->pos)
        r->pos = r->size;
    else
        r->pos += len;
}

static CborError fuzz_transfer_string(void *token, const void **userptr, size_t offset, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    if (!r || !userptr)
        return CborErrorIO;
    if (offset > r->size || len > r->size - offset)
        return CborErrorIO;
    *userptr = r->data + offset;
    return CborNoError;
}

static const struct CborParserOperations fuzz_reader_ops = {
    fuzz_can_read_bytes,
    fuzz_read_bytes,
    fuzz_advance_bytes,
    fuzz_transfer_string
};

static void exercise_iterator(CborValue *it, int depth) {
    if (!it || depth > 8)
        return;

    (void)cbor_value_reparse(it);
    (void)cbor_value_skip_tag(it);
    (void)cbor_value_reparse(it);

    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            int steps = 0;
            while (cbor_value_is_valid(&recursed) && steps < 16) {
                (void)cbor_value_reparse(&recursed);
                (void)cbor_value_skip_tag(&recursed);

                if (!cbor_value_is_valid(&recursed))
                    break;

                exercise_iterator(&recursed, depth + 1);

                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
                steps++;
            }
        }
    }

    if (cbor_value_is_valid(it)) {
        CborValue tmp = *it;
        (void)cbor_value_skip_tag(&tmp);
        (void)cbor_value_reparse(&tmp);
        if (cbor_value_is_valid(&tmp))
            (void)cbor_value_advance(&tmp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    {
        CborParser parser;
        CborValue it;
        uint32_t flags[] = { 0u, 1u, 0xffffffffu, (uint32_t)Size };

        for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
            CborError err = cbor_parser_init(Data, Size, flags[i], &parser, &it);
            if (err == CborNoError) {
                exercise_iterator(&it, 0);

                CborValue walk = it;
                int steps = 0;
                while (cbor_value_is_valid(&walk) && steps < 16) {
                    (void)cbor_value_reparse(&walk);
                    (void)cbor_value_skip_tag(&walk);
                    if (!cbor_value_is_valid(&walk))
                        break;
                    err = cbor_value_advance(&walk);
                    if (err != CborNoError)
                        break;
                    steps++;
                }
            }
        }
    }

    {
        FuzzReader reader;
        CborParser parser;
        CborValue it;

        reader.data = Data;
        reader.size = Size;
        reader.pos = 0;

        CborError err = cbor_parser_init_reader(&fuzz_reader_ops, &parser, &it, &reader);
        if (err == CborNoError) {
            exercise_iterator(&it, 0);

            CborValue walk = it;
            int steps = 0;
            while (cbor_value_is_valid(&walk) && steps < 16) {
                (void)cbor_value_reparse(&walk);
                (void)cbor_value_skip_tag(&walk);
                if (!cbor_value_is_valid(&walk))
                    break;
                err = cbor_value_advance(&walk);
                if (err != CborNoError)
                    break;
                steps++;
            }
        }
    }

    return 0;
}