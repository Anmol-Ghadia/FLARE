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
    if (!it)
        return;

    (void)cbor_value_reparse(it);
    (void)_cbor_value_begin_string_iteration(it);
    (void)cbor_value_reparse(it);
    (void)_cbor_value_finish_string_iteration(it);
    (void)cbor_value_reparse(it);
    (void)cbor_value_advance_fixed(it);
    (void)cbor_value_reparse(it);
    (void)cbor_value_advance(it);
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

    CborError err = cbor_parser_init_reader(&ops, &parser, &it, &reader);
    if (err == CborNoError) {
        exercise_value(&it);

        CborValue copy1 = it;
        exercise_value(&copy1);

        CborValue copy2 = it;
        (void)_cbor_value_begin_string_iteration(&copy2);
        (void)cbor_value_reparse(&copy2);
        (void)cbor_value_advance_fixed(&copy2);
        (void)_cbor_value_finish_string_iteration(&copy2);
        (void)cbor_value_advance(&copy2);

        CborValue copy3 = it;
        for (int i = 0; i < 4; ++i) {
            (void)cbor_value_reparse(&copy3);
            (void)cbor_value_advance_fixed(&copy3);
            (void)cbor_value_advance(&copy3);
        }
    }

    CborValue manual;
    memset(&manual, 0, sizeof(manual));
    manual.parser = &parser;
    manual.source.token = &reader;
    manual.remaining = (Size > 0) ? 1u : 0u;
    manual.type = (Size > 0) ? Data[0] : 0;
    manual.flags = (Size > 1) ? Data[1] : 0;
    manual.extra = (Size > 3) ? (uint16_t)((Data[2] << 8) | Data[3]) : 0;

    exercise_value(&manual);

    return 0;
}