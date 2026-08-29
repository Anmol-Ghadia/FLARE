#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_decoder.h"

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
} InputState;

typedef struct {
    InputState *input;
    uint32_t mode;
    uint32_t state;
    size_t produced;
    size_t limit;
    size_t max_read;
    LHADecoderCallback callback;
    void *callback_data;
} DummyExtra;

struct _LHADecoderType {
    int (*init)(void *extra_data, LHADecoderCallback callback, void *callback_data);
    void (*free)(void *extra_data);
    size_t (*read)(void *extra_data, uint8_t *buf);
    size_t extra_size;
    size_t max_read;
    size_t block_size;
};

static size_t fuzz_input_callback(void *buf, size_t buf_len, void *callback_data)
{
    InputState *in = (InputState *) callback_data;
    size_t remain, n;

    if (in == NULL || buf == NULL) {
        return 0;
    }

    if (in->pos >= in->size) {
        return 0;
    }

    remain = in->size - in->pos;
    n = buf_len < remain ? buf_len : remain;
    memcpy(buf, in->data + in->pos, n);
    in->pos += n;
    return n;
}

static void progress_callback(unsigned int current, unsigned int total, void *callback_data)
{
    volatile unsigned int sink = current ^ total;
    volatile uint8_t *p = (volatile uint8_t *) callback_data;
    if (p != NULL) {
        *p ^= (uint8_t) sink;
    }
}

static int dummy_init(void *extra_data, LHADecoderCallback callback, void *callback_data)
{
    DummyExtra *ex = (DummyExtra *) extra_data;
    uint8_t tmp[16];
    size_t n = 0;

    if (ex == NULL) {
        return 0;
    }

    ex->input = (InputState *) callback_data;
    ex->produced = 0;
    ex->callback = callback;
    ex->callback_data = callback_data;
    ex->max_read = 1;

    if (ex->input != NULL && ex->input->size > 0) {
        ex->mode = ex->input->data[0] % 4;
        ex->state = ex->input->size > 1 ? ex->input->data[1] : 0;
        ex->limit = ex->input->size > 2 ? ex->input->data[2] : 32;
        ex->max_read = 1 + (ex->input->size % 64);
    } else {
        ex->mode = 0;
        ex->state = 0;
        ex->limit = 32;
    }

    if (callback != NULL && ex->input != NULL) {
        n = callback(tmp, sizeof(tmp), ex->input);
        ex->state ^= (uint32_t) n;
    }

    return 1;
}

static void dummy_free(void *extra_data)
{
    DummyExtra *ex = (DummyExtra *) extra_data;
    if (ex != NULL) {
        ex->state ^= 0xA5A5u;
    }
}

static size_t dummy_read(void *extra_data, uint8_t *buf)
{
    DummyExtra *ex = (DummyExtra *) extra_data;
    size_t i, n, remain;

    if (ex == NULL || buf == NULL) {
        return 0;
    }

    if (ex->produced >= ex->limit) {
        return 0;
    }

    remain = ex->limit - ex->produced;
    n = ex->max_read;
    if (n == 0) {
        n = 1;
    }
    if (n > remain) {
        n = remain;
    }

    switch (ex->mode) {
        case 0:
            for (i = 0; i < n; ++i) {
                buf[i] = (uint8_t) (ex->state + i);
            }
            break;

        case 1:
            if (ex->callback == NULL) {
                return 0;
            }
            n = ex->callback(buf, n, ex->callback_data);
            if (n == 0) {
                return 0;
            }
            break;

        case 2:
            n = 1;
            buf[0] = (uint8_t) ex->state;
            break;

        default:
            for (i = 0; i < n; ++i) {
                ex->state = ex->state * 1103515245u + 12345u;
                buf[i] = (uint8_t) (ex->state >> 16);
            }
            break;
    }

    ex->produced += n;
    return n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    InputState in;
    LHADecoder *decoder = NULL;
    struct _LHADecoderType dtype;
    uint64_t stream_length;
    uint8_t progress_state = 0;
    uint8_t outbuf[256];
    size_t offset = 0;
    size_t i, nreads;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    memset(&dtype, 0, sizeof(dtype));
    dtype.init = dummy_init;
    dtype.free = dummy_free;
    dtype.read = dummy_read;
    dtype.extra_size = sizeof(DummyExtra);
    dtype.max_read = 1 + (Size % 64);
    dtype.block_size = (Size > 0) ? (1 + (Size % 1024)) : 1;

    in.data = Data;
    in.size = Size;
    in.pos = 0;

    stream_length = (uint64_t) Size;
    if (Size >= 8) {
        stream_length =
            ((uint64_t) Data[0] << 56) |
            ((uint64_t) Data[1] << 48) |
            ((uint64_t) Data[2] << 40) |
            ((uint64_t) Data[3] << 32) |
            ((uint64_t) Data[4] << 24) |
            ((uint64_t) Data[5] << 16) |
            ((uint64_t) Data[6] << 8)  |
            ((uint64_t) Data[7]);
    }
    stream_length &= 0xffffULL;

    decoder = lha_decoder_new(&dtype, fuzz_input_callback, &in, stream_length);
    if (decoder == NULL) {
        return 0;
    }

    lha_decoder_monitor(decoder, progress_callback, &progress_state);
    (void) lha_decoder_get_crc(decoder);
    (void) lha_decoder_get_length(decoder);

    nreads = (Size > 0) ? (1 + (Data[0] % 16)) : 4;
    for (i = 0; i < nreads; ++i) {
        size_t want;

        if (offset < Size) {
            want = Data[offset++] % sizeof(outbuf);
        } else {
            want = i % sizeof(outbuf);
        }

        if ((i & 1) == 0) {
            lha_decoder_monitor(decoder, progress_callback, &progress_state);
        } else {
            lha_decoder_monitor(decoder, NULL, NULL);
        }

        (void) lha_decoder_get_crc(decoder);
        (void) lha_decoder_get_length(decoder);

        if (want == 0) {
            want = 1;
        }

        (void) lha_decoder_read(decoder, outbuf, want);

        (void) lha_decoder_get_crc(decoder);
        (void) lha_decoder_get_length(decoder);
    }

    while (1) {
        size_t got = lha_decoder_read(decoder, outbuf, sizeof(outbuf));
        (void) lha_decoder_get_crc(decoder);
        (void) lha_decoder_get_length(decoder);
        if (got == 0) {
            break;
        }
    }

    lha_decoder_free(decoder);
    return 0;
}