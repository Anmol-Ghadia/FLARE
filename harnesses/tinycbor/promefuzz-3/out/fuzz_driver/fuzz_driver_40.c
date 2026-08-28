// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_advance at cborparser.c:506:11 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_parser_init_reader at cborparser.c:348:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_skip_tag at cborparser.c:542:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_is_container at cbor.h:367:22 in cbor.h
// cbor_value_enter_container at cborparser.c:569:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
// cbor_value_reparse at cborparser.c:397:11 in cbor.h
// cbor_value_is_valid at cbor.h:379:22 in cbor.h
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
    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_reparse(it) != CborNoError)
        return;
    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_skip_tag(it) != CborNoError)
        return;
    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_reparse(it) != CborNoError)
        return;
    if (!cbor_value_is_valid(it))
        return;

    if (cbor_value_is_container(it)) {
        CborValue recursed;
        CborError err = cbor_value_enter_container(it, &recursed);
        if (err == CborNoError) {
            int steps = 0;
            while (cbor_value_is_valid(&recursed) && steps < 16) {
                if (cbor_value_reparse(&recursed) != CborNoError)
                    break;
                if (!cbor_value_is_valid(&recursed))
                    break;

                if (cbor_value_skip_tag(&recursed) != CborNoError)
                    break;
                if (!cbor_value_is_valid(&recursed))
                    break;

                exercise_iterator(&recursed, depth + 1);

                if (!cbor_value_is_valid(&recursed))
                    break;
                err = cbor_value_advance(&recursed);
                if (err != CborNoError)
                    break;
                steps++;
            }
        }
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

        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));
        if (cbor_parser_init(Data, Size, 0u, &parser, &it) == CborNoError) {
            exercise_iterator(&it, 0);

            if (cbor_value_is_valid(&it)) {
                CborValue tmp = it;
                if (cbor_value_reparse(&tmp) == CborNoError &&
                    cbor_value_is_valid(&tmp) &&
                    cbor_value_skip_tag(&tmp) == CborNoError &&
                    cbor_value_is_valid(&tmp)) {
                    (void)cbor_value_reparse(&tmp);
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

        memset(&parser, 0, sizeof(parser));
        memset(&it, 0, sizeof(it));
        if (cbor_parser_init_reader(&fuzz_reader_ops, &parser, &it, &reader) == CborNoError) {
            exercise_iterator(&it, 0);

            if (cbor_value_is_valid(&it)) {
                CborValue tmp = it;
                if (cbor_value_reparse(&tmp) == CborNoError &&
                    cbor_value_is_valid(&tmp) &&
                    cbor_value_skip_tag(&tmp) == CborNoError &&
                    cbor_value_is_valid(&tmp)) {
                    (void)cbor_value_reparse(&tmp);
                }
            }
        }
    }

    return 0;
}