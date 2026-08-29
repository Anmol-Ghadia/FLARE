#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_reader.h"
#include "lha_decoder.h"

struct FuzzStream {
    const uint8_t *data;
    size_t size;
    size_t pos;
};

static size_t fuzz_decoder_callback(void *buf, size_t buf_len, void *callback_data)
{
    struct FuzzStream *st = (struct FuzzStream *) callback_data;
    size_t remaining, n;

    if (st == NULL || buf == NULL || buf_len == 0) {
        return 0;
    }

    if (st->pos >= st->size) {
        return 0;
    }

    remaining = st->size - st->pos;
    n = remaining < buf_len ? remaining : buf_len;
    memcpy(buf, st->data + st->pos, n);
    st->pos += n;
    return n;
}

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

static void fuzz_decoder_with_name(const char *name,
                                   const uint8_t *data,
                                   size_t size,
                                   uint64_t stream_length)
{
    const LHADecoderType *dtype;
    LHADecoder *decoder;
    struct FuzzStream st;
    uint8_t outbuf[4096];
    volatile unsigned int progress_sink = 0;
    size_t i;

    dtype = lha_decoder_for_name(name);
    if (dtype == NULL) {
        return;
    }

    st.data = data;
    st.size = size;
    st.pos = 0;

    decoder = lha_decoder_new(dtype, fuzz_decoder_callback, &st, stream_length);
    if (decoder == NULL) {
        return;
    }

    lha_decoder_monitor(decoder, fuzz_progress_callback, (void *) &progress_sink);

    for (i = 0; i < 8; ++i) {
        size_t want = sizeof(outbuf);
        size_t got;

        if (size > 0) {
            want = (size_t) data[i % size];
            want %= sizeof(outbuf);
        }

        if (want == 0) {
            want = (i + 1) * 17;
            if (want > sizeof(outbuf)) {
                want = sizeof(outbuf);
            }
        }

        got = lha_decoder_read(decoder, outbuf, want);
        if (got == 0) {
            break;
        }

        if ((i & 1) == 0) {
            lha_decoder_monitor(decoder, fuzz_progress_callback, (void *) &progress_sink);
        }
    }

    lha_decoder_free(decoder);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    char *name;
    size_t name_len;
    uint64_t stream_length = 0;
    static const char *const names[] = {
        "-lh0-",
        "-lh1-",
        "-lh4-",
        "-lh5-",
        "-lh6-",
        "-lh7-",
        "-lzs-",
        "-lz4-",
        "-pm0-",
        "-pm1-",
        ""
    };
    size_t i;

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                (void) fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    name_len = Size < 64 ? Size : 64;
    name = (char *) malloc(name_len + 1);
    if (name == NULL) {
        return 0;
    }

    if (name_len > 0) {
        memcpy(name, Data, name_len);
    }
    name[name_len] = '\0';

    (void) lha_decoder_for_name(name);

    if (Size >= 8) {
        size_t j;
        for (j = 0; j < 8; ++j) {
            stream_length = (stream_length << 8) | Data[j];
        }
    } else {
        stream_length = (uint64_t) Size;
    }

    fuzz_decoder_with_name(name, Data, Size, stream_length);
    fuzz_decoder_with_name(name, Data, Size, (uint64_t) Size);
    fuzz_decoder_with_name(name, Data, Size, 0);

    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        fuzz_decoder_with_name(names[i], Data, Size, stream_length);
    }

    free(name);
    return 0;
}