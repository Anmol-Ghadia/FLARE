// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_decoder_new64 at lha_decoder.c:71:13 in lha_decoder.h
// lha_decoder_get_crc at lha_decoder.c:277:10 in lha_decoder.h
// lha_decoder_get_length64 at lha_decoder.c:295:10 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_get_crc at lha_decoder.c:277:10 in lha_decoder.h
// lha_decoder_get_length64 at lha_decoder.c:295:10 in lha_decoder.h
// lha_decoder_read at lha_decoder.c:199:8 in lha_decoder.h
// lha_decoder_get_crc at lha_decoder.c:277:10 in lha_decoder.h
// lha_decoder_get_length64 at lha_decoder.c:295:10 in lha_decoder.h
// lha_decoder_free at lha_decoder.c:140:6 in lha_decoder.h
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

int LLVMFuzzerTestOneInput_6(const uint8_t *Data, size_t Size)
{
    static const LHADecoderType *decoder_types[] = {
#ifdef LHAD_TYPE_LH0
        &LHAD_TYPE_LH0,
#endif
#ifdef LHAD_TYPE_LH1
        &LHAD_TYPE_LH1,
#endif
#ifdef LHAD_TYPE_LH4
        &LHAD_TYPE_LH4,
#endif
#ifdef LHAD_TYPE_LH5
        &LHAD_TYPE_LH5,
#endif
#ifdef LHAD_TYPE_LH6
        &LHAD_TYPE_LH6,
#endif
#ifdef LHAD_TYPE_LH7
        &LHAD_TYPE_LH7,
#endif
#ifdef LHAD_TYPE_LZS
        &LHAD_TYPE_LZS,
#endif
#ifdef LHAD_TYPE_LZ4
        &LHAD_TYPE_LZ4,
#endif
#ifdef LHAD_TYPE_LHARK
        &LHAD_TYPE_LHARK,
#endif
        NULL
    };

    uint8_t outbuf[512];
    size_t i, j;
    FILE *fp;
    FuzzStream stream;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    for (i = 0; decoder_types[i] != NULL; ++i) {
        uint64_t stream_length;
        LHADecoder *dec;

        memset(&stream, 0, sizeof(stream));
        stream.data = Data;
        stream.size = Size;
        stream.pos = 0;

        if (Size >= 8) {
            stream_length =
                ((uint64_t) Data[0]) |
                ((uint64_t) Data[1] << 8) |
                ((uint64_t) Data[2] << 16) |
                ((uint64_t) Data[3] << 24) |
                ((uint64_t) Data[4] << 32) |
                ((uint64_t) Data[5] << 40) |
                ((uint64_t) Data[6] << 48) |
                ((uint64_t) Data[7] << 56);
        } else {
            stream_length = (uint64_t) Size;
        }

        dec = lha_decoder_new(decoder_types[i], fuzz_decoder_callback, &stream, stream_length);
        if (dec == NULL) {
            continue;
        }

        (void) lha_decoder_get_crc(dec);
        (void) lha_decoder_get_length(dec);

        for (j = 0; j < 16; ++j) {
            size_t want;

            if (Size > j) {
                want = (size_t) (Data[j] % sizeof(outbuf));
            } else {
                want = (j * 31U) % sizeof(outbuf);
            }

            (void) lha_decoder_read(dec, outbuf, want);
            (void) lha_decoder_get_crc(dec);
            (void) lha_decoder_get_length(dec);
        }

        (void) lha_decoder_read(dec, outbuf, sizeof(outbuf));
        (void) lha_decoder_get_crc(dec);
        (void) lha_decoder_get_length(dec);

        lha_decoder_free(dec);
    }

    return 0;
}