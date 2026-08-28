// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_negative_int at cborencoder.c:361:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
// cbor_encode_tag at cborencoder.c:432:11 in cbor.h
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

static uint64_t load_u64_prefix(const uint8_t *data, size_t size, size_t start)
{
    uint64_t v = 0;
    size_t i;
    for (i = start; i < size && i < start + 8; ++i)
        v = (v << 8) | data[i];
    return v;
}

int LLVMFuzzerTestOneInput_35(const uint8_t *Data, size_t Size)
{
    CborEncoder enc_buf;
    CborEncoder enc_writer;
    CborError err;
    uint8_t *outbuf;
    size_t outsz;
    size_t text_len;
    uint64_t absval;
    uint64_t tag;
    char *zstr = NULL;
    FILE *fp = NULL;
    int flags = 0;

    if (!Data && Size != 0)
        return 0;

    outsz = Size + 32;
    if (outsz < 32)
        outsz = 32;

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

    /* Use only valid fixed-buffer encoder flags to avoid writer-mode misconfiguration. */
    cbor_encoder_init(&enc_buf, outbuf, outsz, 0);

    absval = load_u64_prefix(Data, Size, 0);
    tag = load_u64_prefix(Data, Size, 8);
    text_len = Size;

    if (Size > 0) {
        err = cbor_encode_text_string(&enc_buf, (const char *)Data, text_len);
        (void)err;

        err = cbor_encode_text_string(&enc_buf, (const char *)Data, text_len / 2);
        (void)err;
    } else {
        err = cbor_encode_text_string(&enc_buf, "", 0);
        (void)err;
    }

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

        if (Size > 0) {
            err = cbor_encode_text_string(&enc_writer, (const char *)Data, text_len);
            (void)err;
        } else {
            err = cbor_encode_text_string(&enc_writer, "", 0);
            (void)err;
        }

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
    (void)flags;
    return 0;
}