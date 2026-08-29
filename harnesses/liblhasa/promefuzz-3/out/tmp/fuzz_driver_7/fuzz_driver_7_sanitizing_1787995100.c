#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_reader.h"
#include "lha_decoder.h"

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
} FuzzStream;

static size_t fuzz_decoder_callback(void *buf, size_t buf_len, void *callback_data)
{
    FuzzStream *s = (FuzzStream *) callback_data;
    size_t remain, n;

    if (s == NULL || buf == NULL) {
        return 0;
    }

    if (s->pos >= s->size) {
        return 0;
    }

    remain = s->size - s->pos;
    n = buf_len < remain ? buf_len : remain;
    memcpy(buf, s->data + s->pos, n);
    s->pos += n;

    return n;
}

static uint64_t read_u64_le(const uint8_t *p, size_t n)
{
    uint64_t v = 0;
    size_t i, lim = n < 8 ? n : 8;

    for (i = 0; i < lim; ++i) {
        v |= ((uint64_t) p[i]) << (8 * i);
    }

    return v;
}

static void fuzz_decoder_paths(const uint8_t *Data, size_t Size)
{
    char namebuf[64];
    size_t name_len, i;
    const LHADecoderType *dtype;
    FuzzStream stream;
    LHADecoder *decoder;
    uint8_t outbuf[256];
    uint64_t stream_length;
    size_t offset = 0;
    int rounds;

    dtype = lha_decoder_for_name("");
    if (dtype != NULL) {
        stream.data = Data;
        stream.size = Size;
        stream.pos = 0;
        decoder = lha_decoder_new(dtype, fuzz_decoder_callback, &stream, 0);
        if (decoder != NULL) {
            (void) lha_decoder_get_length(decoder);
            (void) lha_decoder_read(decoder, outbuf, sizeof(outbuf));
            (void) lha_decoder_get_length(decoder);
            lha_decoder_free(decoder);
        }
    }

    if (Size == 0) {
        return;
    }

    name_len = Data[0] % (sizeof(namebuf) - 1);
    if (1 + name_len > Size) {
        name_len = Size > 1 ? (Size - 1) : 0;
    }

    for (i = 0; i < name_len; ++i) {
        unsigned char c = Data[1 + i];
        if (c == '\0') {
            c = 'A';
        }
        namebuf[i] = (char) c;
    }
    namebuf[name_len] = '\0';
    offset = 1 + name_len;

    dtype = lha_decoder_for_name(namebuf);
    if (dtype == NULL) {
        static const char *fallbacks[] = {
            "-lh0-",
            "-lh1-",
            "-lh4-",
            "-lh5-",
            "-lh6-",
            "-lh7-",
            "-lzs-",
            "-lz4-",
            "-pm0-"
        };
        size_t idx = 0;

        if (offset < Size) {
            idx = Data[offset] % (sizeof(fallbacks) / sizeof(fallbacks[0]));
            offset++;
        }

        dtype = lha_decoder_for_name(fallbacks[idx]);
    }

    if (dtype == NULL) {
        return;
    }

    stream_length = (offset < Size) ? read_u64_le(Data + offset, Size - offset) : 0;
    if (offset < Size) {
        offset += (Size - offset >= 8) ? 8 : (Size - offset);
    }

    stream.data = (offset < Size) ? (Data + offset) : Data;
    stream.size = (offset < Size) ? (Size - offset) : 0;
    stream.pos = 0;

    decoder = lha_decoder_new(dtype, fuzz_decoder_callback, &stream, stream_length);
    if (decoder == NULL) {
        return;
    }

    (void) lha_decoder_get_length(decoder);

    rounds = 0;
    while (rounds < 16) {
        size_t req;
        size_t got;

        if (Size > 0) {
            req = (size_t) (Data[rounds % Size] % sizeof(outbuf));
        } else {
            req = 0;
        }

        if ((rounds % 4) == 0) {
            req = sizeof(outbuf);
        }

        got = lha_decoder_read(decoder, outbuf, req);
        (void) lha_decoder_get_length(decoder);

        if (got == 0 && req != 0) {
            break;
        }

        rounds++;
    }

    (void) lha_decoder_read(decoder, outbuf, 0);
    (void) lha_decoder_get_length(decoder);

    lha_decoder_free(decoder);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    fuzz_decoder_paths(Data, Size);

    return 0;
}