// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_parser_init_reader at cborparser.c:348:11 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// _cbor_value_begin_string_iteration at cborparser.c:960:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_advance_fixed at cborparser.c:454:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
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

static bool iterator_looks_valid(const CborValue *it) {
    if (!it)
        return false;
    if (!it->parser)
        return false;
    if (it->remaining == 0)
        return false;
    return true;
}

static void try_reparse(CborValue *it) {
    if (!iterator_looks_valid(it))
        return;
    (void)cbor_value_reparse(it);
}

static void try_begin_finish_string_iteration(CborValue *it) {
    if (!iterator_looks_valid(it))
        return;
    (void)_cbor_value_begin_string_iteration(it);
    (void)cbor_value_reparse(it);
    if (iterator_looks_valid(it))
        (void)_cbor_value_finish_string_iteration(it);
}

static void try_advance_fixed_copy(const CborValue *it) {
    if (!iterator_looks_valid(it))
        return;
    CborValue tmp = *it;
    (void)cbor_value_advance_fixed(&tmp);
}

static void try_advance_copy(const CborValue *it) {
    if (!iterator_looks_valid(it))
        return;
    CborValue tmp = *it;
    (void)cbor_value_advance(&tmp);
}

static void exercise_value(CborValue *it) {
    if (!iterator_looks_valid(it))
        return;

    try_reparse(it);
    try_begin_finish_string_iteration(it);
    try_reparse(it);
    try_advance_fixed_copy(it);
    try_advance_copy(it);
    try_reparse(it);
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

    if (cbor_parser_init_reader(&ops, &parser, &it, &reader) != CborNoError)
        return 0;

    exercise_value(&it);

    {
        CborValue copy1 = it;
        exercise_value(&copy1);
    }

    {
        CborValue copy2 = it;
        if (iterator_looks_valid(&copy2)) {
            (void)_cbor_value_begin_string_iteration(&copy2);
            try_reparse(&copy2);
            if (iterator_looks_valid(&copy2))
                (void)_cbor_value_finish_string_iteration(&copy2);
        }
    }

    {
        CborValue copy3 = it;
        for (int i = 0; i < 4; ++i) {
            if (!iterator_looks_valid(&copy3))
                break;
            if (cbor_value_reparse(&copy3) != CborNoError)
                break;
            if (!iterator_looks_valid(&copy3))
                break;
            if (cbor_value_advance(&copy3) != CborNoError)
                break;
        }
    }

    return 0;
}