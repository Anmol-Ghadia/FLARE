// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encoder_create_map at cborencoder.c:561:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_undefined at cbor.h:259:27 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_undefined at cbor.h:259:27 in cbor.h
// cbor_encoder_init_writer at cborencoder.c:203:6 in cbor.h
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_simple_value at cborencoder.c:388:11 in cbor.h
// cbor_encode_boolean at cbor.h:255:27 in cbor.h
// cbor_encode_null at cbor.h:257:27 in cbor.h
// cbor_encode_undefined at cbor.h:259:27 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "cbor.h"

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType appendType)
{
    (void)appendType;
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

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    fp = fopen("./dummy_file", "ab");
    if (!fp)
        return 0;

    {
        CborEncoder encoder;
        cbor_encoder_init_writer(&encoder, fuzz_writer, fp);

        size_t i = 0;
        while (i < Size) {
            uint8_t op = Data[i++];

            switch (op % 6) {
            case 0: {
                uint8_t v = (i < Size) ? Data[i++] : op;
                (void)cbor_encode_simple_value(&encoder, v);
                break;
            }
            case 1: {
                bool b = (op & 1) != 0;
                if (i < Size)
                    b = (Data[i++] & 1) != 0;
                (void)cbor_encode_boolean(&encoder, b);
                break;
            }
            case 2:
                (void)cbor_encode_null(&encoder);
                break;
            case 3:
                (void)cbor_encode_undefined(&encoder);
                break;
            case 4: {
                uint8_t v1 = (i < Size) ? Data[i++] : 20;
                uint8_t v2 = (i < Size) ? Data[i++] : 21;
                (void)cbor_encode_simple_value(&encoder, v1);
                (void)cbor_encode_simple_value(&encoder, v2);
                break;
            }
            case 5: {
                uint8_t v = (i < Size) ? Data[i++] : 0xff;
                (void)cbor_encode_simple_value(&encoder, v);
                (void)cbor_encode_boolean(&encoder, (v & 1) != 0);
                (void)cbor_encode_null(&encoder);
                (void)cbor_encode_undefined(&encoder);
                break;
            }
            }
        }
    }

    {
        CborEncoder parent;
        CborEncoder child;
        cbor_encoder_init_writer(&parent, fuzz_writer, fp);

        CborError err = cbor_encoder_create_array(&parent, &child, CborIndefiniteLength);
        if (err == CborNoError) {
            if (Size > 0)
                (void)cbor_encode_simple_value(&child, Data[0]);
            if (Size > 1)
                (void)cbor_encode_boolean(&child, (Data[1] & 1) != 0);
            if (Size > 2)
                (void)cbor_encode_null(&child);
            if (Size > 3)
                (void)cbor_encode_undefined(&child);
            (void)cbor_encoder_close_container(&parent, &child);
        }
    }

    {
        CborEncoder parent;
        CborEncoder child;
        cbor_encoder_init_writer(&parent, fuzz_writer, fp);

        size_t map_len = (Size > 0) ? (Data[0] % 3) : 0;
        CborError err = cbor_encoder_create_map(&parent, &child, map_len);
        if (err == CborNoError) {
            for (size_t j = 0; j < map_len; ++j) {
                (void)cbor_encode_simple_value(&child, (uint8_t)j);
                if (j & 1)
                    (void)cbor_encode_boolean(&child, true);
                else
                    (void)cbor_encode_null(&child);
            }
            (void)cbor_encoder_close_container(&parent, &child);
        }
    }

    fclose(fp);
    return 0;
}