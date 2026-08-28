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
    uint8_t scratch[256];
} FuzzReader;

static bool fuzz_can_read_bytes(void *token, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    return r && r->pos <= r->size && len <= (r->size - r->pos);
}

static void *fuzz_read_bytes(void *token, void *dst, size_t offset, size_t len) {
    FuzzReader *r = (FuzzReader *)token;
    if (!r || offset > r->size || len > (r->size - offset))
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
    if (!r || !userptr || offset > r->size || len > (r->size - offset))
        return CborErrorIO;
    *userptr = r->data + offset;
    return CborNoError;
}

static void exercise_value(CborValue *it, int depth) {
    if (!it || depth > 8)
        return;
    if (!cbor_value_is_valid(it))
        return;

    (void)cbor_value_reparse(it);
    (void)cbor_value_skip_tag(it);
    if (!cbor_value_is_valid(it))
        return;
    (void)cbor_value_reparse(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError && cbor_value_is_valid(&recursed)) {
            for (int i = 0; i < 16 && cbor_value_is_valid(&recursed); ++i) {
                exercise_value(&recursed, depth + 1);
                if (!cbor_value_is_valid(&recursed))
                    break;
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    for (uint32_t flags = 0; flags < 4; ++flags) {
        CborParser parser;
        CborValue it;
        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));

        CborError err = cbor_parser_init(Data, Size, flags, &parser, &it);
        if (err == CborNoError && cbor_value_is_valid(&it)) {
            exercise_value(&it, 0);

            CborValue tmp = it;
            for (int i = 0; i < 8 && cbor_value_is_valid(&tmp); ++i) {
                (void)cbor_value_skip_tag(&tmp);
                (void)cbor_value_reparse(&tmp);
                if (!cbor_value_is_valid(&tmp))
                    break;
                err = cbor_value_advance(&tmp);
                if (err != CborNoError)
                    break;
            }
        }
    }

    {
        static const struct CborParserOperations ops = {
            fuzz_can_read_bytes,
            fuzz_read_bytes,
            fuzz_advance_bytes,
            fuzz_transfer_string
        };
        FuzzReader reader;
        CborParser parser;
        CborValue it;

        reader.data = Data;
        reader.size = Size;
        reader.pos = 0;
        memset(reader.scratch, 0, sizeof(reader.scratch));
        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));

        CborError err = cbor_parser_init_reader(&ops, &parser, &it, &reader);
        if (err == CborNoError && cbor_value_is_valid(&it)) {
            exercise_value(&it, 0);

            CborValue tmp = it;
            for (int i = 0; i < 8 && cbor_value_is_valid(&tmp); ++i) {
                (void)cbor_value_reparse(&tmp);
                (void)cbor_value_skip_tag(&tmp);
                if (!cbor_value_is_valid(&tmp))
                    break;
                err = cbor_value_advance(&tmp);
                if (err != CborNoError)
                    break;
            }
        }
    }

    return 0;
}