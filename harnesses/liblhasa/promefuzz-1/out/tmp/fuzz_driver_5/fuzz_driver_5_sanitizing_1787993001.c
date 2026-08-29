#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_decoder.h"

struct InputState {
    const uint8_t *data;
    size_t size;
    size_t pos;
};

static size_t fuzz_input_callback(void *buf, size_t buf_len, void *callback_data)
{
    struct InputState *st = (struct InputState *) callback_data;
    size_t remain, n;

    if (st == NULL || buf == NULL || buf_len == 0) {
        return 0;
    }

    if (st->pos >= st->size) {
        return 0;
    }

    remain = st->size - st->pos;
    n = remain < buf_len ? remain : buf_len;
    memcpy(buf, st->data + st->pos, n);
    st->pos += n;
    return n;
}

static void fuzz_progress_callback(unsigned int current, unsigned int total, void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    static const char *const known_names[] = {
        "-lh0-",
        "-lh1-",
        "-lh2-",
        "-lh3-",
        "-lh4-",
        "-lh5-",
        "-lh6-",
        "-lh7-",
        "-lzs-",
        "-lz4-",
        "-lz5-"
    };

    struct InputState st;
    char namebuf[16];
    const LHADecoderType *dtype = NULL;
    LHADecoder *decoder = NULL;
    uint8_t *readbuf = NULL;
    uint64_t stream_length = 0;
    volatile unsigned int progress_sink = 0;
    size_t offset = 0;
    size_t i, rounds;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    memset(namebuf, 0, sizeof(namebuf));

    if (Size > 0) {
        size_t name_len = Data[0] % (sizeof(namebuf) - 1);
        if (name_len > 0 && Size >= 1 + name_len) {
            memcpy(namebuf, Data + 1, name_len);
            namebuf[name_len] = '\0';
            offset = 1 + name_len;
        } else {
            offset = 1;
        }
    }

    dtype = lha_decoder_for_name(namebuf);

    if (dtype == NULL) {
        if (Size > offset) {
            dtype = lha_decoder_for_name(
                known_names[Data[offset] % (sizeof(known_names) / sizeof(known_names[0]))]
            );
            offset++;
        } else {
            dtype = lha_decoder_for_name(known_names[0]);
        }
    }

    if (dtype == NULL) {
        return 0;
    }

    if (Size >= offset + 8) {
        for (i = 0; i < 8; i++) {
            stream_length = (stream_length << 8) | Data[offset + i];
        }
        offset += 8;
    } else {
        stream_length = (uint64_t) Size;
    }

    if (offset > Size) {
        offset = Size;
    }

    st.data = Data + offset;
    st.size = Size - offset;
    st.pos = 0;

    decoder = lha_decoder_new(dtype, fuzz_input_callback, &st, stream_length);
    if (decoder == NULL) {
        return 0;
    }

    lha_decoder_monitor(decoder, fuzz_progress_callback, (void *) &progress_sink);
    (void) lha_decoder_get_length(decoder);

    readbuf = (uint8_t *) malloc(65536);
    if (readbuf == NULL) {
        lha_decoder_free(decoder);
        return 0;
    }

    rounds = 1;
    if (st.size > 0) {
        rounds += st.data[0] % 32;
    }

    for (i = 0; i < rounds; i++) {
        size_t req = 0;
        size_t got;

        if (st.pos < st.size) {
            req = (size_t) st.data[st.pos++];
        }

        req <<= 8;
        if (st.pos < st.size) {
            req |= st.data[st.pos++];
        }

        req %= 65536;

        got = lha_decoder_read(decoder, readbuf, req);
        (void) got;
        (void) lha_decoder_get_length(decoder);

        if ((i & 1) == 0) {
            lha_decoder_monitor(decoder, fuzz_progress_callback, (void *) &progress_sink);
        }

        if (got == 0 && req != 0) {
            break;
        }
    }

    (void) lha_decoder_read(decoder, readbuf, 0);
    (void) lha_decoder_get_length(decoder);

    free(readbuf);
    lha_decoder_free(decoder);
    return 0;
}