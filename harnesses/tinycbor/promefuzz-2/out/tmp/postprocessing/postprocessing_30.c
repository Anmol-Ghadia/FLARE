#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "cbor.h"

static size_t consume_size_t(const uint8_t *Data, size_t Size, size_t *Offset) {
    size_t v = 0;
    size_t i;
    for (i = 0; i < sizeof(size_t) && *Offset < Size; ++i, ++(*Offset))
        v = (v << 8) | Data[*Offset];
    return v;
}

static uint64_t consume_u64(const uint8_t *Data, size_t Size, size_t *Offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *Offset < Size; ++i, ++(*Offset))
        v = (v << 8) | Data[*Offset];
    return v;
}

static int64_t consume_i64(const uint8_t *Data, size_t Size, size_t *Offset) {
    return (int64_t)consume_u64(Data, Size, Offset);
}

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType type) {
    FILE *fp = (FILE *)token;
    (void)type;
    if (fp && data && len > 0) {
        if (fwrite(data, 1, len, fp) != len)
            return CborErrorIO;
    }
    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    uint8_t outbuf[512];
    uint8_t outbuf2[256];
    CborEncoder enc;
    CborEncoder child;
    CborEncoder writer_enc;
    CborError err;
    FILE *fp = NULL;

    memset(outbuf, 0, sizeof(outbuf));
    memset(outbuf2, 0, sizeof(outbuf2));
    memset(&enc, 0, sizeof(enc));
    memset(&child, 0, sizeof(child));
    memset(&writer_enc, 0, sizeof(writer_enc));

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    fp = fopen("./dummy_file", "ab+");
    if (fp)
        cbor_encoder_init_writer(&writer_enc, fuzz_writer, fp);
    else
        memset(&writer_enc, 0, sizeof(writer_enc));

    {
        size_t bufsize = (Size > 0) ? (consume_size_t(Data, Size, &off) % sizeof(outbuf)) : 0;
        cbor_encoder_init(&enc, outbuf, bufsize, 0);
    }

    err = cbor_encode_tag(&enc, (CborTag)consume_u64(Data, Size, &off));
    (void)err;
    err = cbor_encode_negative_int(&enc, consume_u64(Data, Size, &off));
    (void)err;
    err = cbor_encode_int(&enc, consume_i64(Data, Size, &off));
    (void)err;

    {
        size_t length = consume_size_t(Data, Size, &off);
        err = cbor_encoder_create_array(&enc, &child, length);
        if (err == CborNoError) {
            size_t iterations = (Size - off) % 16;
            size_t i;
            for (i = 0; i < iterations; ++i) {
                if (off >= Size)
                    break;
                switch (Data[off++] % 3) {
                    case 0:
                        (void)cbor_encode_int(&child, consume_i64(Data, Size, &off));
                        break;
                    case 1:
                        (void)cbor_encode_negative_int(&child, consume_u64(Data, Size, &off));
                        break;
                    default:
                        (void)cbor_encode_tag(&child, (CborTag)consume_u64(Data, Size, &off));
                        break;
                }
            }
            (void)cbor_encoder_close_container(&enc, &child);
        }
    }

    if (fp) {
        err = cbor_encode_tag(&writer_enc, (CborTag)consume_u64(Data, Size, &off));
        (void)err;
        err = cbor_encode_negative_int(&writer_enc, consume_u64(Data, Size, &off));
        (void)err;
        err = cbor_encode_int(&writer_enc, consume_i64(Data, Size, &off));
        (void)err;

        {
            size_t length = consume_size_t(Data, Size, &off);
            err = cbor_encoder_create_array(&writer_enc, &child, length);
            if (err == CborNoError) {
                size_t iterations = (Size - off) % 16;
                size_t i;
                for (i = 0; i < iterations; ++i) {
                    CborEncoder nested;
                    if (off >= Size)
                        break;
                    switch (Data[off++] % 4) {
                        case 0:
                            (void)cbor_encode_int(&child, consume_i64(Data, Size, &off));
                            break;
                        case 1:
                            (void)cbor_encode_negative_int(&child, consume_u64(Data, Size, &off));
                            break;
                        case 2:
                            (void)cbor_encode_tag(&child, (CborTag)consume_u64(Data, Size, &off));
                            break;
                        default:
                            memset(&nested, 0, sizeof(nested));
                            if (cbor_encoder_create_array(&child, &nested, consume_size_t(Data, Size, &off)) == CborNoError) {
                                (void)cbor_encode_int(&nested, consume_i64(Data, Size, &off));
                                (void)cbor_encode_negative_int(&nested, consume_u64(Data, Size, &off));
                                (void)cbor_encoder_close_container(&child, &nested);
                            }
                            break;
                    }
                }
                (void)cbor_encoder_close_container(&writer_enc, &child);
            }
        }
    }

    {
        size_t bufsize2 = (Size > off) ? (consume_size_t(Data, Size, &off) % sizeof(outbuf2)) : 0;
        CborEncoder enc2;
        CborEncoder arr2;
        memset(&enc2, 0, sizeof(enc2));
        memset(&arr2, 0, sizeof(arr2));
        cbor_encoder_init(&enc2, outbuf2, bufsize2, 0);
        (void)cbor_encode_int(&enc2, consume_i64(Data, Size, &off));
        (void)cbor_encode_negative_int(&enc2, consume_u64(Data, Size, &off));
        (void)cbor_encode_tag(&enc2, (CborTag)consume_u64(Data, Size, &off));
        if (cbor_encoder_create_array(&enc2, &arr2, consume_size_t(Data, Size, &off)) == CborNoError) {
            (void)cbor_encode_tag(&arr2, (CborTag)consume_u64(Data, Size, &off));
            (void)cbor_encode_int(&arr2, consume_i64(Data, Size, &off));
            (void)cbor_encoder_close_container(&enc2, &arr2);
        }
    }

    if (fp)
        fclose(fp);

    return 0;
}