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
} FuzzReaderToken;

static bool fuzz_can_read_bytes(void *token, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t)
        return false;
    return t->pos + len <= t->size;
}

static void *fuzz_read_bytes(void *token, void *dst, size_t offset, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t || offset > t->size || len > t->size - offset)
        return NULL;

    const uint8_t *src = t->data + offset;
    if (dst) {
        memcpy(dst, src, len);
        return dst;
    }

    if (len > sizeof(t->scratch))
        len = sizeof(t->scratch);
    memcpy(t->scratch, src, len);
    return t->scratch;
}

static void fuzz_advance_bytes(void *token, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t)
        return;
    if (len > t->size - t->pos)
        t->pos = t->size;
    else
        t->pos += len;
}

static CborError fuzz_transfer_string(void *token, const void **userptr, size_t offset, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t || !userptr)
        return CborErrorIO;
    if (offset > t->size || len > t->size - offset)
        return CborErrorUnexpectedEOF;
    *userptr = t->data + offset;
    return CborNoError;
}

static const struct CborParserOperations fuzz_ops = {
    fuzz_can_read_bytes,
    fuzz_read_bytes,
    fuzz_advance_bytes,
    fuzz_transfer_string
};

static void exercise_iterator(CborValue *it) {
    CborError err;

    if (!it)
        return;

    (void)cbor_value_reparse(it);

    (void)_cbor_value_begin_string_iteration(it);
    (void)cbor_value_reparse(it);
    (void)_cbor_value_finish_string_iteration(it);

    for (int i = 0; i < 8; ++i) {
        if (it->remaining == 0)
            break;

        err = cbor_value_reparse(it);
        if (err != CborNoError)
            break;

        err = cbor_value_advance_fixed(it);
        if (err == CborNoError)
            continue;

        if (err == CborErrorAdvancePastEOF)
            break;

        if (it->remaining == 0)
            break;

        err = cbor_value_advance(it);
        if (err != CborNoError)
            break;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    FuzzReaderToken token;
    token.data = Data;
    token.size = Size;
    token.pos = 0;
    memset(token.scratch, 0, sizeof(token.scratch));

    CborParser parser;
    CborValue it;
    memset(&parser, 0, sizeof(parser));
    memset(&it, 0, sizeof(it));

    CborError err = cbor_parser_init_reader(&fuzz_ops, &parser, &it, &token);

    (void)cbor_value_reparse(&it);
    (void)_cbor_value_begin_string_iteration(&it);
    (void)_cbor_value_finish_string_iteration(&it);

    if (err == CborNoError && it.remaining > 0) {
        CborValue a = it;
        CborValue b = it;
        CborValue c = it;
        CborValue d = it;

        exercise_iterator(&a);

        (void)_cbor_value_begin_string_iteration(&b);
        exercise_iterator(&b);

        exercise_iterator(&c);
        (void)_cbor_value_finish_string_iteration(&c);

        (void)cbor_value_reparse(&d);
        if (d.remaining > 0) {
            CborError e = cbor_value_advance_fixed(&d);
            if (e != CborNoError && e != CborErrorAdvancePastEOF && d.remaining > 0)
                (void)cbor_value_advance(&d);
        }
    }

    return 0;
}