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

#include "lha_reader.h"
#include "lha_decoder.h"

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t pos;
} FuzzStream;

static int fuzz_decoder_callback(void *callback_data, uint8_t *buf, size_t buf_len)
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

    return (int) n;
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

    if (Size == 0) {
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
        size_t req = 0;
        size_t got;

        if (offset + (size_t) rounds < Size) {
            req = (size_t) (Data[(offset + (size_t) rounds) % Size] % sizeof(outbuf));
        } else {
            req = (size_t) (rounds * 17) % sizeof(outbuf);
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

static void fuzz_reader_read_path(const uint8_t *Data, size_t Size)
{
    LHAReader reader;
    LHABasicReader basic;
    LHAFileHeader filehdr;
    uint8_t buf[128];
    size_t n;

    memset(&reader, 0, sizeof(reader));
    memset(&basic, 0, sizeof(basic));
    memset(&filehdr, 0, sizeof(filehdr));

    filehdr.length = Size > 8 ? read_u64_le(Data, Size) : (uint64_t) Size;
    filehdr.compressed_length = Size > 16 ? read_u64_le(Data + 8, Size - 8) : (uint64_t) Size;

    if (Size > 0) {
        memcpy(filehdr.compress_method, "-lh0-", 6);
    } else {
        memcpy(filehdr.compress_method, "-lhd-", 6);
    }

    basic.curr_file = &filehdr;
    basic.curr_file_remaining = Size;
    basic.eof = 0;

    reader.reader = &basic;
    reader.curr_file = &filehdr;
    reader.decoder = NULL;
    reader.inner_decoder = NULL;

    n = lha_reader_read(&reader, buf, sizeof(buf));
    (void) n;

    n = lha_reader_read(&reader, buf, 0);
    (void) n;

    if (reader.decoder != NULL) {
        lha_decoder_free(reader.decoder);
        reader.decoder = NULL;
    }

    if (reader.inner_decoder != NULL && reader.inner_decoder != reader.decoder) {
        lha_decoder_free(reader.inner_decoder);
        reader.inner_decoder = NULL;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    fuzz_decoder_paths(Data, Size);
    fuzz_reader_read_path(Data, Size);

    return 0;
}