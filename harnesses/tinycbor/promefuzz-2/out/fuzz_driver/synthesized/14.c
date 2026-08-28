// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_at_end at cbor.h:360:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_parser_init_reader at cborparser.c:348:11 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_leave_container at cborparser.c:618:11 in cbor.h
// cbor_value_validate_basic at cborparser.c:433:11 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// _cbor_value_finish_string_iteration at cborparser.c:973:11 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
    if (!t || !t->data || t->pos > t->size || offset > (t->size - t->pos) ||
        len > (t->size - t->pos - offset))
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
    if (t->pos > t->size)
        t->pos = t->size;
    else if (len > t->size - t->pos)
        t->pos = t->size;
    else
        t->pos += len;
}

static CborError fuzz_transfer_string(void *token, const void **userptr, size_t offset, size_t len) {
    FuzzReaderToken *t = (FuzzReaderToken *)token;
    if (!t || !userptr || !t->data || t->pos > t->size || offset > (t->size - t->pos) ||
        len > (t->size - t->pos - offset))
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

static int cbor_value_seems_valid(const CborValue *it) {
    return it && it->type != CborInvalidType;
}

static void safe_validate_basic(CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return;
    (void)cbor_value_validate_basic(it);
}

static void safe_reparse(CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return;
    (void)cbor_value_reparse(it);
}

static void safe_finish_string_iteration(CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return;
    CborValue tmp = *it;
    (void)_cbor_value_finish_string_iteration(&tmp);
}

static CborError safe_advance(CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return CborErrorIllegalType;
    return cbor_value_advance(it);
}

static int safe_is_container(const CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return 0;
    return cbor_value_is_container(it);
}

static int safe_at_end(const CborValue *it) {
    if (!cbor_value_seems_valid(it))
        return 1;
    return cbor_value_at_end(it);
}

static void traverse_value(CborValue *it, int depth) {
    if (!cbor_value_seems_valid(it) || depth > 8)
        return;

    safe_reparse(it);
    safe_validate_basic(it);

    if (safe_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError && cbor_value_seems_valid(&recursed)) {
            int count = 0;
            while (!safe_at_end(&recursed) && count < 32) {
                safe_validate_basic(&recursed);
                safe_reparse(&recursed);

                if (!cbor_value_seems_valid(&recursed))
                    break;

                if (safe_is_container(&recursed)) {
                    traverse_value(&recursed, depth + 1);
                } else {
                    safe_finish_string_iteration(&recursed);
                    CborValue copy = recursed;
                    (void)safe_advance(&copy);
                }

                err = safe_advance(&recursed);
                if (err != CborNoError)
                    break;
                count++;
            }

            if (cbor_value_seems_valid(&recursed) && safe_at_end(&recursed))
                (void)cbor_value_leave_container(it, &recursed);
        }
    } else {
        safe_finish_string_iteration(it);
        CborValue copy = *it;
        (void)safe_advance(&copy);
    }
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    FuzzReaderToken token;
    token.data = Data;
    token.size = Size;
    token.pos = 0;

    CborParser parser;
    memset(&parser, 0, sizeof(parser));

    CborValue it;
    memset(&it, 0, sizeof(it));
    it.type = CborInvalidType;

    CborError err = cbor_parser_init_reader(&fuzz_ops, &parser, &it, &token);
    if (err != CborNoError || !cbor_value_seems_valid(&it))
        return 0;

    safe_validate_basic(&it);
    safe_reparse(&it);
    traverse_value(&it, 0);

    if (!cbor_value_seems_valid(&it))
        return 0;

    CborValue iter = it;
    for (int i = 0; i < 16; i++) {
        if (!cbor_value_seems_valid(&iter))
            break;

        safe_validate_basic(&iter);
        safe_reparse(&iter);

        if (!cbor_value_seems_valid(&iter))
            break;

        if (safe_is_container(&iter)) {
            CborValue child;
            CborError e2 = cbor_value_enter_container(&iter, &child);
            if (e2 == CborNoError && cbor_value_seems_valid(&child)) {
                int inner = 0;
                while (!safe_at_end(&child) && inner < 8) {
                    safe_validate_basic(&child);
                    safe_reparse(&child);

                    if (!cbor_value_seems_valid(&child))
                        break;

                    safe_finish_string_iteration(&child);
                    e2 = safe_advance(&child);
                    if (e2 != CborNoError)
                        break;
                    inner++;
                }
                if (cbor_value_seems_valid(&child) && safe_at_end(&child))
                    (void)cbor_value_leave_container(&iter, &child);
            }
        } else {
            safe_finish_string_iteration(&iter);
        }

        if (safe_advance(&iter) != CborNoError)
            break;
    }

    return 0;
}