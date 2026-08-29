#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lha_decoder.h"

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
} DummyExtra;

static size_t fuzz_input_callback(void *callback_data, void *buf, size_t buf_len) {
    FuzzCtx *ctx = (FuzzCtx *) callback_data;
    if (ctx == NULL || buf == NULL || buf_len == 0) {
        return 0;
    }

    if (ctx->pos >= ctx->size) {
        return 0;
    }

    size_t remain = ctx->size - ctx->pos;
    size_t n = remain < buf_len ? remain : buf_len;

    memcpy(buf, ctx->data + ctx->pos, n);
    ctx->pos += n;
    return n;
}

static int dummy_init(void *extra_data, LHADecoderCallback callback, void *callback_data) {
    DummyExtra *extra = (DummyExtra *) extra_data;
    FuzzCtx *ctx = (FuzzCtx *) callback_data;
    uint8_t tmp[32];
    size_t want, got;

    if (extra == NULL) {
        return 0;
    }

    memset(extra, 0, sizeof(*extra));
    extra->ctx = ctx;
    extra->state = 0x12345678u;

    if (ctx != NULL) {
        ctx->init_reads++;
    }

    if (callback != NULL) {
        want = (ctx != NULL && ctx->size > 0) ? ((ctx->data[0] % sizeof(tmp)) + 1) : sizeof(tmp);
        got = callback(callback_data, tmp, want);
        for (size_t i = 0; i < got; i++) {
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
    }
}

static size_t dummy_read(void *extra_data, uint8_t *buf) {
    DummyExtra *extra = (DummyExtra *) extra_data;
    FuzzCtx *ctx;
    uint8_t tmp[256];
    size_t max_out, req, got, i;

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
        max_out = (size_t)(ctx->data[1] % sizeof(tmp)) + 1;
    }

    req = max_out;
    got = fuzz_input_callback(ctx, tmp, req);

    if (got == 0) {
        if ((ctx->flags & 4u) && extra->produced < 16) {
            buf[0] = (uint8_t)(extra->state & 0xffu);
            extra->state = extra->state * 1103515245u + 12345u;
            extra->produced++;
            return 1;
        }
        return 0;
    }

    for (i = 0; i < got; i++) {
        extra->state = (extra->state * 1664525u) + 1013904223u + tmp[i];
        buf[i] = (uint8_t)(tmp[i] ^ (uint8_t)(extra->state >> 24));
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FuzzCtx ctx;
    LHADecoderType dtype;
    LHADecoder *decoder = NULL;
    uint8_t stack_buf[512];
    uint8_t *heap_buf = NULL;
    uint64_t stream_length;
    size_t offset = 0;
    size_t iterations, i;

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
    if (Size > 0) {
        ctx.flags = Data[0];
        offset = 1;
    }

    memset(&dtype, 0, sizeof(dtype));
    dtype.init = dummy_init;
    dtype.free = dummy_free;
    dtype.read = dummy_read;
    dtype.extra_size = sizeof(DummyExtra);
    dtype.max_read = (Size > offset) ? ((size_t)(Data[offset] % 128) + 1) : 64;
    if (Size > offset) {
        offset++;
    }
    dtype.block_size = (Size > offset) ? ((size_t)(Data[offset] % 64) + 1) : 16;
    if (Size > offset) {
        offset++;
    }

    stream_length = 0;
    for (i = 0; i < 8 && offset + i < Size; i++) {
        stream_length |= ((uint64_t) Data[offset + i]) << (8 * i);
    }
    if (Size >= offset + 8) {
        offset += 8;
    } else {
        offset = Size;
    }

    if (ctx.flags & 8u) {
        stream_length = (uint64_t) Size;
    } else if (ctx.flags & 16u) {
        stream_length = 0;
    }

    decoder = lha_decoder_new(&dtype, fuzz_input_callback, &ctx, stream_length);
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

    iterations = 1;
    if (offset < Size) {
        iterations = (Size - offset) < 64 ? (Size - offset) : 64;
    }

    for (i = 0; i < iterations; i++) {
        size_t req;
        size_t got;

        if (offset + i < Size) {
            req = (size_t)(Data[offset + i]);
        } else {
            req = i;
        }

        switch (req % 4) {
            case 0:
                req = 0;
                got = lha_decoder_read(decoder, stack_buf, req);
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