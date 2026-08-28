#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
} FuzzReaderToken;

static bool fuzz_can_read_bytes(void *token, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    return t && t->pos <= t->size && len <= (t->size - t->pos);
}

static void *fuzz_read_bytes(void *token, void *dst, size_t offset, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t || t->pos > t->size || offset > (t->size - t->pos) || len > (t->size - t->pos - offset))
        return NULL;

    const uint8_t *src = t->data + t->pos + offset;
    if (dst) {
        memcpy(dst, src, len);
        return dst;
    }
    return (void *)src;
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
    if (!t || !userptr || t->pos > t->size || offset > (t->size - t->pos) || len > (t->size - t->pos - offset))
        return CborErrorIO;
    *userptr = t->data + t->pos + offset;
    return CborNoError;
}

static const struct CborParserOperations fuzz_ops = {
    .can_read_bytes = fuzz_can_read_bytes,
    .read_bytes = fuzz_read_bytes,
    .advance_bytes = fuzz_advance_bytes,
    .transfer_string = fuzz_transfer_string
};

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void exercise_string_finish(CborValue *it) {
    CborValue tmp = *it;
    (void)_cbor_value_finish_string_iteration(&tmp);
}

static void traverse_value(CborValue *it, int depth) {
    if (!it || depth > 8)
        return;

    (void)cbor_value_reparse(it);
    (void)cbor_value_validate_basic(it);

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            int count = 0;
            while (!cbor_value_at_end(&recursed) && count < 32) {
                (void)cbor_value_validate_basic(&recursed);
                (void)cbor_value_reparse(&recursed);

                if (cbor_value_is_container(&recursed)) {
                    traverse_value(&recursed, depth + 1);
                } else {
                    exercise_string_finish(&recursed);
                    CborValue copy = recursed;
                    (void)cbor_value_advance(&copy);
                }

                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
                count++;
            }

            if (cbor_value_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    } else {
        exercise_string_finish(it);
        CborValue copy = *it;
        (void)cbor_value_advance(&copy);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    FuzzReaderToken token;
    token.data = Data;
    token.size = Size;
    token.pos = 0;

    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init_reader(&fuzz_ops, &parser, &it, &token);
    if (err != CborNoError) {
        (void)cbor_value_reparse(&it);
        (void)cbor_value_validate_basic(&it);
        (void)_cbor_value_finish_string_iteration(&it);
        (void)cbor_value_advance(&it);
        return 0;
    }

    (void)cbor_value_validate_basic(&it);
    (void)cbor_value_reparse(&it);
    traverse_value(&it, 0);

    CborValue iter = it;
    for (int i = 0; i < 16; i++) {
        (void)cbor_value_validate_basic(&iter);
        (void)cbor_value_reparse(&iter);

        if (cbor_value_is_container(&iter)) {
            CborValue child;
            CborError e2 = cbor_value_enter_container(&iter, &child);
            if (e2 == CborNoError) {
                int inner = 0;
                while (!cbor_value_at_end(&child) && inner < 8) {
                    (void)cbor_value_validate_basic(&child);
                    (void)cbor_value_reparse(&child);
                    exercise_string_finish(&child);
                    e2 = cbor_value_advance(&child);
                    if (e2 != CborNoError)
                        break;
                    inner++;
                }
                if (cbor_value_at_end(&child))
                    (void)cbor_value_leave_container(&iter, &child);
            }
        } else {
            exercise_string_finish(&iter);
        }

        if (cbor_value_advance(&iter) != CborNoError)
            break;
    }

    return 0;
}