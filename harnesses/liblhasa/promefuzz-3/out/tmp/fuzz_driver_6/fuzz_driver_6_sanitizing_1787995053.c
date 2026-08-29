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

static size_t fuzz_decoder_callback(void *callback_data, uint8_t *buf, size_t buf_len)
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

static int dummy_init(void *extra_data, LHADecoderCallback callback, void *callback_data)
{
    (void) extra_data;
    (void) callback;
    (void) callback_data;
    return 1;
}

static void dummy_free(void *extra_data)
{
    (void) extra_data;
}

static size_t dummy_read_zero(void *extra_data, uint8_t *buf)
{
    (void) extra_data;
    (void) buf;
    return 0;
}

static size_t dummy_read_pattern(void *extra_data, uint8_t *buf)
{
    FuzzStream *s = (FuzzStream *) extra_data;
    size_t i, n;

    if (buf == NULL) {
        return 0;
    }

    n = 16;
    if (s != NULL && s->size > 0) {
        n = (s->data[0] % 32) + 1;
    }

    for (i = 0; i < n; ++i) {
        uint8_t v = 0;
        if (s != NULL && s->pos < s->size) {
            v = s->data[s->pos++];
        } else {
            v = (uint8_t) i;
        }
        buf[i] = v;
    }

    return n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    uint8_t outbuf[256];
    size_t i, nreads, want;
    FILE *fp;
    FuzzStream cb_stream;
    FuzzStream extra_stream;
    LHADecoder *dec = NULL;
    LHAReader reader;
    LHABasicReader basic_reader;
    LHAFileHeader file_header;
    LHADecoderType dtype_zero;
    LHADecoderType dtype_pattern;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    memset(&dtype_zero, 0, sizeof(dtype_zero));
    dtype_zero.init = dummy_init;
    dtype_zero.free = dummy_free;
    dtype_zero.read = dummy_read_zero;
    dtype_zero.extra_size = 0;
    dtype_zero.max_read = 1;
    dtype_zero.block_size = 1;

    memset(&dtype_pattern, 0, sizeof(dtype_pattern));
    dtype_pattern.init = dummy_init;
    dtype_pattern.free = dummy_free;
    dtype_pattern.read = dummy_read_pattern;
    dtype_pattern.extra_size = sizeof(FuzzStream);
    dtype_pattern.max_read = 32;
    dtype_pattern.block_size = 8;

    memset(&cb_stream, 0, sizeof(cb_stream));
    cb_stream.data = Data;
    cb_stream.size = Size;
    cb_stream.pos = 0;

    dec = lha_decoder_new(&dtype_zero, fuzz_decoder_callback, &cb_stream, (uint64_t) Size);
    if (dec != NULL) {
        (void) lha_decoder_get_crc(dec);
        (void) lha_decoder_get_length(dec);

        nreads = Size > 0 ? (Data[0] % 8) + 1 : 1;
        for (i = 0; i < nreads; ++i) {
            want = Size > (i + 1) ? (size_t) (Data[i + 1] % sizeof(outbuf)) : sizeof(outbuf);
            (void) lha_decoder_read(dec, outbuf, want);
            (void) lha_decoder_get_crc(dec);
            (void) lha_decoder_get_length(dec);
        }

        lha_decoder_free(dec);
        dec = NULL;
    }

    memset(&extra_stream, 0, sizeof(extra_stream));
    extra_stream.data = Data;
    extra_stream.size = Size;
    extra_stream.pos = 0;

    dec = lha_decoder_new(&dtype_pattern, fuzz_decoder_callback, &cb_stream,
                          Size > 1 ? ((uint64_t) Data[1] << 8) | Data[0] : (uint64_t) Size);
    if (dec != NULL) {
        if (dtype_pattern.extra_size >= sizeof(FuzzStream)) {
            memcpy((void *) (dec + 1), &extra_stream, sizeof(FuzzStream));
        }

        (void) lha_decoder_get_crc(dec);
        (void) lha_decoder_get_length(dec);

        nreads = Size > 2 ? (Data[2] % 16) + 1 : 4;
        for (i = 0; i < nreads; ++i) {
            want = Size > (i + 3) ? (size_t) (Data[i + 3] % sizeof(outbuf)) : (i * 17) % sizeof(outbuf);
            (void) lha_decoder_read(dec, outbuf, want);
            (void) lha_decoder_get_crc(dec);
            (void) lha_decoder_get_length(dec);
        }

        lha_decoder_free(dec);
        dec = NULL;
    }

    memset(&reader, 0, sizeof(reader));
    memset(&basic_reader, 0, sizeof(basic_reader));
    memset(&file_header, 0, sizeof(file_header));

    file_header.length = Size > 3 ? (uint64_t) Data[3] : 0;
    file_header.compressed_length = Size;
    memcpy(file_header.compress_method, "-lh0-", 6);

    basic_reader.curr_file = &file_header;
    basic_reader.curr_file_remaining = Size;
    basic_reader.eof = 0;

    reader.reader = &basic_reader;
    reader.curr_file = &file_header;
    reader.decoder = NULL;
    reader.inner_decoder = NULL;

    (void) lha_reader_read(&reader, outbuf, 0);
    (void) lha_reader_read(&reader, outbuf, sizeof(outbuf));
    if (Size > 4) {
        (void) lha_reader_read(&reader, outbuf, (size_t) (Data[4] % sizeof(outbuf)));
    }

    if (reader.decoder != NULL) {
        lha_decoder_free(reader.decoder);
        reader.decoder = NULL;
    }
    if (reader.inner_decoder != NULL && reader.inner_decoder != reader.decoder) {
        lha_decoder_free(reader.inner_decoder);
        reader.inner_decoder = NULL;
    }

    return 0;
}