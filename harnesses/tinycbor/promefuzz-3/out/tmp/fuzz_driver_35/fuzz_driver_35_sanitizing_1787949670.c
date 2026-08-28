#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType append_type)
{
    (void)append_type;
    FILE *fp = (FILE *)token;
    if (!fp)
        return CborErrorIO;

    if (len == 0)
        return CborNoError;

    if (!data)
        return CborErrorIO;

    if (fwrite(data, 1, len, fp) != len)
        return CborErrorIO;

    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    CborEncoder enc_buf;
    CborEncoder enc_writer;
    CborError err;
    uint8_t *outbuf;
    size_t outsz;
    size_t text_len;
    uint64_t absval = 0;
    uint64_t tag = 0;
    char *zstr = NULL;
    FILE *fp = NULL;
    int flags = 0;
    size_t i;

    if (!Data && Size != 0)
        return 0;

    outsz = Size ? Size : 1;
    outbuf = (uint8_t *)malloc(outsz);
    if (!outbuf)
        return 0;

    if (Size >= 4) {
        flags = (int)((unsigned)Data[0] |
                      ((unsigned)Data[1] << 8) |
                      ((unsigned)Data[2] << 16) |
                      ((unsigned)Data[3] << 24));
    } else if (Size > 0) {
        flags = (int)Data[0];
    }

    cbor_encoder_init(&enc_buf, outbuf, outsz, flags);

    for (i = 0; i < Size && i < 8; ++i)
        absval = (absval << 8) | Data[i];

    for (i = 8; i < Size && i < 16; ++i)
        tag = (tag << 8) | Data[i];
    if (Size < 8) {
        for (i = 0; i < Size; ++i)
            tag = (tag << 8) | Data[i];
    }

    text_len = Size;
    err = cbor_encode_text_string(&enc_buf, (const char *)Data, text_len);
    (void)err;

    err = cbor_encode_text_string(&enc_buf, (const char *)Data, text_len / 2);
    (void)err;

    err = cbor_encode_negative_int(&enc_buf, absval);
    (void)err;

    err = cbor_encode_negative_int(&enc_buf, 0);
    (void)err;

    err = cbor_encode_tag(&enc_buf, (CborTag)tag);
    (void)err;

    err = cbor_encode_tag(&enc_buf, 0);
    (void)err;

    zstr = (char *)malloc(Size + 1);
    if (zstr) {
        if (Size)
            memcpy(zstr, Data, Size);
        zstr[Size] = '\0';

        err = cbor_encode_text_stringz(&enc_buf, zstr);
        (void)err;

        if (Size > 0) {
            zstr[Size / 2] = '\0';
            err = cbor_encode_text_stringz(&enc_buf, zstr);
            (void)err;
        }
    }

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    fp = fopen("./dummy_file", "ab+");
    if (fp) {
        cbor_encoder_init_writer(&enc_writer, fuzz_writer, fp);

        err = cbor_encode_text_string(&enc_writer, (const char *)Data, text_len);
        (void)err;

        if (zstr) {
            err = cbor_encode_text_stringz(&enc_writer, zstr);
            (void)err;
        }

        err = cbor_encode_negative_int(&enc_writer, absval);
        (void)err;

        err = cbor_encode_negative_int(&enc_writer, 0);
        (void)err;

        err = cbor_encode_tag(&enc_writer, (CborTag)tag);
        (void)err;

        err = cbor_encode_tag(&enc_writer, (CborTag)absval);
        (void)err;

        fclose(fp);
    }

    free(zstr);
    free(outbuf);
    return 0;
}