// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_decoder_new64 at lha_decoder.c:71:13 in lha_decoder.h
// lha_decoder_monitor at lha_decoder.c:171:6 in lha_decoder.h
// lha_decoder_get_crc at lha_decoder.c:277:10 in lha_decoder.h
// lha_decoder_get_length64 at lha_decoder.c:295:10 in lha_decoder.h
// lha_decoder_free at lha_decoder.c:140:6 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_get_crc at lha_decoder.c:277:10 in lha_decoder.h
// lha_decoder_get_length64 at lha_decoder.c:295:10 in lha_decoder.h
// lha_decoder_monitor at lha_decoder.c:171:6 in lha_decoder.h
// lha_decoder_free at lha_decoder.c:140:6 in lha_decoder.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lha_decoder.h"

struct _LHADecoderType {
    int (*init)(void *extra_data, LHADecoderCallback callback, void *callback_data);
    void (*free)(void *extra_data);
    size_t (*read)(void *extra_data, uint8_t *buf);
    size_t extra_size;
    size_t max_read;
    size_t block_size;
};

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
    uint32_t flags;
    size_t init_reads;
    size_t read_calls;
    size_t progress_calls;
} FuzzCtx;

typedef struct {
    FuzzCtx *ctx;
    uint32_t state;
    size_t produced;
    LHADecoderCallback cb;
    void *cb_data;
} DummyExtra;

static size_t fuzz_input_callback(void *buf, size_t buf_len, void *callback_data) {
    FuzzCtx *ctx = (FuzzCtx *) callback_data;
    size_t n;

    if (ctx == NULL || buf == NULL || buf_len == 0) {
        return 0;
    }

    if (ctx->pos >= ctx->size) {
        return 0;
    }

    n = ctx->size - ctx->pos;
    if (n > buf_len) {
        n = buf_len;
    }

    memcpy(buf, ctx->data + ctx->pos, n);
    ctx->pos += n;
    return n;
}

static int dummy_init(void *extra_data, LHADecoderCallback callback, void *callback_data) {
    DummyExtra *extra = (DummyExtra *) extra_data;
    FuzzCtx *ctx = (FuzzCtx *) callback_data;
    uint8_t tmp[32];
    size_t want, got, i;

    if (extra == NULL) {
        return 0;
    }

    memset(extra, 0, sizeof(*extra));
    extra->ctx = ctx;
    extra->state = 0x12345678u;
    extra->cb = callback;
    extra->cb_data = callback_data;

    if (ctx != NULL) {
        ctx->init_reads++;
    }

    if (callback != NULL) {
        want = (ctx != NULL && ctx->size > 0) ? ((size_t) (ctx->data[0] % sizeof(tmp)) + 1) : sizeof(tmp);
        got = callback(tmp, want, callback_data);
        for (i = 0; i < got; i++) {
            extra->state = (extra->state * 33u) ^ tmp[i];
        }
    }

    if (ctx != NULL && (ctx->flags & 1u)) {
        return 0;
    }

    return 1;
}

static void dummy_free(void *extra_data) {
    DummyExtra *extra = (DummyExtra *) extra_data;
    if (extra != NULL) {
        extra->state ^= 0xdeadbeefu;
        extra->produced = 0;
        extra->ctx = NULL;
        extra->cb = NULL;
        extra->cb_data = NULL;
    }
}

static size_t dummy_read(void *extra_data, uint8_t *buf) {
    DummyExtra *extra = (DummyExtra *) extra_data;
    FuzzCtx *ctx;
    uint8_t tmp[256];
    size_t max_out, got, i;

    if (extra == NULL || buf == NULL) {
        return 0;
    }

    ctx = extra->ctx;
    if (ctx == NULL) {
        return 0;
    }

    ctx->read_calls++;

    if ((ctx->flags & 2u) && (ctx->read_calls % 5u == 0u)) {
        return 0;
    }

    max_out = 64;
    if (ctx->size > 1) {
        max_out = (size_t) (ctx->data[1] % sizeof(tmp)) + 1;
    }

    if (extra->cb == NULL) {
        return 0;
    }

    got = extra->cb(tmp, max_out, extra->cb_data);

    if (got == 0) {
        if ((ctx->flags & 4u) && extra->produced < 16) {
            buf[0] = (uint8_t) (extra->state & 0xffu);
            extra->state = extra->state * 1103515245u + 12345u;
            extra->produced++;
            return 1;
        }
        return 0;
    }

    for (i = 0; i < got; i++) {
        extra->state = (extra->state * 1664525u) + 1013904223u + tmp[i];
        buf[i] = (uint8_t) (tmp[i] ^ (uint8_t) (extra->state >> 24));
    }

    extra->produced += got;
    return got;
}

static void progress_cb(unsigned int current, unsigned int total, void *callback_data) {
    FuzzCtx *ctx = (FuzzCtx *) callback_data;
    (void) current;
    (void) total;
    if (ctx != NULL) {
        ctx->progress_calls++;
    }
}

int LLVMFuzzerTestOneInput_3(const uint8_t *Data, size_t Size) {
    FuzzCtx ctx;
    struct _LHADecoderType dtype;
    LHADecoder *decoder;
    uint8_t stack_buf[512];
    uint8_t *heap_buf;
    uint64_t stream_length;
    size_t offset, iterations, i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    memset(&ctx, 0, sizeof(ctx));
    ctx.data = Data;
    ctx.size = Size;
    ctx.pos = 0;
    ctx.flags = (Size > 0) ? Data[0] : 0;

    memset(&dtype, 0, sizeof(dtype));
    dtype.init = dummy_init;
    dtype.free = dummy_free;
    dtype.read = dummy_read;
    dtype.extra_size = sizeof(DummyExtra);
    dtype.max_read = (Size > 1) ? ((size_t) (Data[1] % 128) + 1) : 64;
    dtype.block_size = (Size > 2) ? ((size_t) (Data[2] % 64) + 1) : 16;

    offset = 3;
    stream_length = 0;
    for (i = 0; i < 8 && offset + i < Size; i++) {
        stream_length |= ((uint64_t) Data[offset + i]) << (8 * i);
    }

    if (ctx.flags & 8u) {
        stream_length = (uint64_t) Size;
    } else if (ctx.flags & 16u) {
        stream_length = 0;
    }

    decoder = lha_decoder_new((const LHADecoderType *) &dtype, fuzz_input_callback, &ctx, stream_length);
    if (decoder == NULL) {
        return 0;
    }

    lha_decoder_monitor(decoder, progress_cb, &ctx);
    (void) lha_decoder_get_crc(decoder);
    (void) lha_decoder_get_length(decoder);

    heap_buf = (uint8_t *) malloc(1024);
    if (heap_buf == NULL) {
        lha_decoder_free(decoder);
        return 0;
    }

    if (offset + 8 <= Size) {
        offset += 8;
    } else {
        offset = Size;
    }

    iterations = (offset < Size) ? (Size - offset) : 1;
    if (iterations > 64) {
        iterations = 64;
    }

    for (i = 0; i < iterations; i++) {
        size_t req;
        size_t got;

        req = (offset + i < Size) ? (size_t) Data[offset + i] : i;

        switch (req % 4) {
            case 0:
                got = lha_decoder_read(decoder, stack_buf, 0);
                break;
            case 1:
                req = (req % sizeof(stack_buf)) + 1;
                got = lha_decoder_read(decoder, stack_buf, req);
                break;
            case 2:
                req = (req % 1024) + 1;
                got = lha_decoder_read(decoder, heap_buf, req);
                break;
            default:
                req = dtype.max_read + (req % 32);
                if (req > 1024) {
                    req = 1024;
                }
                got = lha_decoder_read(decoder, heap_buf, req);
                break;
        }

        (void) got;
        (void) lha_decoder_get_crc(decoder);
        (void) lha_decoder_get_length(decoder);

        if ((i % 3) == 0) {
            lha_decoder_monitor(decoder, progress_cb, &ctx);
        }
    }

    free(heap_buf);
    lha_decoder_free(decoder);
    return 0;
}