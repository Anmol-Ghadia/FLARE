// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encoder_close_container_checked at cborencoder_close_container_checked.c:48:11 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType appendType)
{
    FILE *fp = (FILE *)token;
    (void)appendType;

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
    CborEncoder encoder;
    CborEncoder arrayEncoder;
    CborError err;
    FILE *fp;
    char *textz = NULL;
    size_t split1 = 0, split2 = 0, split3 = 0;
    size_t array_len, text_len, byte_len;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;

    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
        fflush(fp);
    }
    fclose(fp);

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;

    cbor_encoder_init_writer(&encoder, fuzz_writer, fp);

    if (Size >= 3) {
        split1 = Data[0] % (Size + 1);
        split2 = Data[1] % (Size + 1);
        split3 = Data[2] % (Size + 1);
    } else if (Size == 2) {
        split1 = Data[0] % (Size + 1);
        split2 = Data[1] % (Size + 1);
        split3 = Size;
    } else if (Size == 1) {
        split1 = Data[0] % (Size + 1);
        split2 = Size;
        split3 = Size;
    }

    if (split2 < split1) {
        size_t t = split1;
        split1 = split2;
        split2 = t;
    }
    if (split3 < split2) {
        size_t t = split2;
        split2 = split3;
        split3 = t;
        if (split2 < split1) {
            t = split1;
            split1 = split2;
            split2 = t;
        }
    }

    array_len = (Size > 0) ? (Data[0] % 8) : 0;
    err = cbor_encoder_create_array(&encoder, &arrayEncoder, array_len);
    if (err == CborNoError) {
        text_len = split1;
        if (text_len > Size)
            text_len = Size;
        (void)cbor_encode_text_string(&arrayEncoder, (const char *)Data, text_len);

        if (split2 > split1) {
            size_t n = split2 - split1;
            textz = (char *)malloc(n + 1);
            if (textz) {
                memcpy(textz, Data + split1, n);
                textz[n] = '\0';
                (void)cbor_encode_text_stringz(&arrayEncoder, textz);
                free(textz);
                textz = NULL;
            }
        } else {
            (void)cbor_encode_text_stringz(&arrayEncoder, "");
        }

        if (split3 > split2) {
            byte_len = split3 - split2;
            (void)cbor_encode_byte_string(&arrayEncoder, Data + split2, byte_len);
        } else {
            (void)cbor_encode_byte_string(&arrayEncoder, Data, 0);
        }

        if (Size > split3) {
            size_t extra_len = Size - split3;
            (void)cbor_encode_text_string(&arrayEncoder, (const char *)(Data + split3), extra_len);
        }

        (void)cbor_encoder_close_container_checked(&encoder, &arrayEncoder);
    }

    cbor_encoder_init_writer(&encoder, fuzz_writer, fp);
    err = cbor_encoder_create_array(&encoder, &arrayEncoder, CborIndefiniteLength);
    if (err == CborNoError) {
        if (Size > 0)
            (void)cbor_encode_byte_string(&arrayEncoder, Data, Size);
        if (Size > 1)
            (void)cbor_encode_text_string(&arrayEncoder, (const char *)Data, Size / 2);
        if (Size > 2) {
            size_t off = Size / 2;
            size_t n = Size - off;
            textz = (char *)malloc(n + 1);
            if (textz) {
                memcpy(textz, Data + off, n);
                textz[n] = '\0';
                (void)cbor_encode_text_stringz(&arrayEncoder, textz);
                free(textz);
                textz = NULL;
            }
        }
        (void)cbor_encoder_close_container_checked(&encoder, &arrayEncoder);
    }

    fclose(fp);
    return 0;
}