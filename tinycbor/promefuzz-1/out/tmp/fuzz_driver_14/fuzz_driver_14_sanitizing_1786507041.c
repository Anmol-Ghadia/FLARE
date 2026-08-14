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
        return CborErrorUnexpectedEOF;
    *userptr = r->data + offset;
    return CborNoError;
}

static void exercise_value(CborValue *it) {
    CborError err;

    if (!it)
        return;

    err = cbor_value_reparse(it);
    if (err != CborNoError)
        return;

    (void)_cbor_value_begin_string_iteration(it);
    (void)cbor_value_reparse(it);
    (void)_cbor_value_finish_string_iteration(it);
    (void)cbor_value_reparse(it);

    if (it->remaining != 0) {
        CborValue tmp = *it;
        (void)cbor_value_advance_fixed(&tmp);
    }

    if (it->remaining != 0) {
        CborValue tmp = *it;
        (void)cbor_value_advance(&tmp);
    }

    (void)cbor_value_reparse(it);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    static const struct CborParserOperations ops = {
        fuzz_can_read_bytes,
        fuzz_read_bytes,
        fuzz_advance_bytes,
        fuzz_transfer_string
    };

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    FuzzReader reader;
    reader.data = Data;
    reader.size = Size;
    reader.pos = 0;
    memset(reader.scratch, 0, sizeof(reader.scratch));

    CborParser parser;
    CborValue it;
    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    if (cbor_parser_init_reader(&ops, &parser, &it, &reader) == CborNoError) {
        exercise_value(&it);

        CborValue copy1 = it;
        exercise_value(&copy1);

        CborValue copy2 = it;
        (void)_cbor_value_begin_string_iteration(&copy2);
        (void)cbor_value_reparse(&copy2);
        (void)_cbor_value_finish_string_iteration(&copy2);
        if (copy2.remaining != 0) {
            CborValue tmp = copy2;
            (void)cbor_value_advance(&tmp);
        }

        CborValue copy3 = it;
        for (int i = 0; i < 4; ++i) {
            if (cbor_value_reparse(&copy3) != CborNoError)
                break;
            if (copy3.remaining == 0)
                break;
            {
                CborValue tmp = copy3;
                (void)cbor_value_advance(&tmp);
                copy3 = tmp;
            }
        }
    }

    return 0;
}