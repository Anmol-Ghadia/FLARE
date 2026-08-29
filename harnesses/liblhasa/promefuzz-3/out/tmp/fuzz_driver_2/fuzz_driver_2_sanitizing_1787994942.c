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

static size_t fuzz_callback(void *buf, size_t buf_len, void *user_data)
{
    struct CallbackState {
        const uint8_t *data;
        size_t size;
        size_t pos;
    };
    struct CallbackState *st = (struct CallbackState *) user_data;
    size_t remain, n;

    if (st == NULL || buf == NULL || buf_len == 0) {
        return 0;
    }

    if (st->pos >= st->size) {
        return 0;
    }

    remain = st->size - st->pos;
    n = buf_len < remain ? buf_len : remain;
    memcpy(buf, st->data + st->pos, n);
    st->pos += n;
    return n;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    struct CallbackState {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } st;
    char namebuf[64];
    size_t name_len;
    const LHADecoderType *dtype = NULL;
    LHADecoder *decoder = NULL;
    uint64_t stream_length = 0;
    uint8_t outbuf[256];
    size_t off = 0;
    size_t i, nreads;
    FILE *fp;
    LHAReader reader;
    LHABasicReader basic_reader;
    LHAFileHeader file_header;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    memset(&reader, 0, sizeof(reader));
    memset(&basic_reader, 0, sizeof(basic_reader));
    memset(&file_header, 0, sizeof(file_header));

    reader.reader = &basic_reader;
    reader.curr_file = &file_header;
    reader.decoder = NULL;
    reader.inner_decoder = NULL;

    basic_reader.curr_file = &file_header;
    basic_reader.curr_file_remaining = Size;
    basic_reader.eof = 0;

    memcpy(file_header.compress_method, "-lh0-", 6);
    file_header.length = Size;
    file_header.compressed_length = Size;

    (void) lha_reader_read(&reader, outbuf, 0);
    (void) lha_reader_read(&reader, outbuf, sizeof(outbuf));

    name_len = Size < sizeof(namebuf) - 1 ? Size : sizeof(namebuf) - 1;
    if (name_len > 0) {
        memcpy(namebuf, Data, name_len);
    }
    namebuf[name_len] = '\0';

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
            "-lz5-"
        };

        for (i = 0; i < sizeof(fallbacks) / sizeof(fallbacks[0]); ++i) {
            dtype = lha_decoder_for_name(fallbacks[i]);
            if (dtype != NULL) {
                break;
            }
        }
    }

    if (dtype != NULL) {
        if (Size >= 8) {
            for (i = 0; i < 8; ++i) {
                stream_length = (stream_length << 8) | Data[i];
            }
            off = 8;
        } else {
            stream_length = Size;
            off = 0;
        }

        st.data = Data + off;
        st.size = Size - off;
        st.pos = 0;

        decoder = lha_decoder_new(dtype, fuzz_callback, &st, stream_length);
        if (decoder != NULL) {
            (void) lha_decoder_get_length(decoder);

            nreads = 0;
            while (nreads < 32) {
                size_t req;
                size_t got;

                if (st.pos < st.size) {
                    req = (size_t)(st.data[st.pos] % (sizeof(outbuf) + 1));
                } else {
                    req = sizeof(outbuf);
                }

                got = lha_decoder_read(decoder, outbuf, req);
                (void) lha_decoder_get_length(decoder);

                if (got == 0) {
                    break;
                }

                nreads++;
            }

            (void) lha_decoder_read(decoder, outbuf, 0);
            (void) lha_decoder_get_length(decoder);
            lha_decoder_free(decoder);
        }
    }

    return 0;
}