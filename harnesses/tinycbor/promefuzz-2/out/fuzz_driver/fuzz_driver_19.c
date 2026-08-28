// This fuzz driver is generated for library tinycbor, aiming to fuzz the following functions:
// cbor_encoder_create_array at cborencoder.c:537:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encoder_close_container at cborencoder.c:580:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encoder_get_buffer_size at cbor.h:280:24 in cbor.h
// cbor_parser_init at cborparser.c:336:11 in cbor.h
// cbor_value_get_type at cbor.h:381:26 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// cbor_value_text_string_equals at cborparser.c:1314:11 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// _cbor_value_get_string_chunk at cborparser.c:1137:11 in cbor.h
// _cbor_value_copy_string at cborparser.c:1286:11 in cbor.h
// _cbor_value_copy_string at cborparser.c:1286:11 in cbor.h
// _cbor_value_copy_string at cborparser.c:1286:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_byte_string at cborencoder.c:463:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_text_string at cborencoder.c:475:11 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
// cbor_encode_text_stringz at cbor.h:249:27 in cbor.h
// cbor_encoder_init at cborencoder.c:195:6 in cbor.h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

static int is_string_type(CborType t) {
    return t == CborByteStringType || t == CborTextStringType;
}

static void fuzz_parsed_value(const uint8_t *buf, size_t len, const uint8_t *cmp_data, size_t cmp_len) {
    CborParser parser;
    CborValue it;
    CborError err = cbor_parser_init(buf, len, 0, &parser, &it);
    if (err != CborNoError)
        return;

    CborType type = cbor_value_get_type(&it);

    if (type == CborTextStringType) {
        bool result = false;
        char *cmp = (char *)malloc(cmp_len + 1);
        if (cmp) {
            if (cmp_len)
                memcpy(cmp, cmp_data, cmp_len);
            cmp[cmp_len] = '\0';
            (void)cbor_value_text_string_equals(&it, cmp, &result);
            free(cmp);
        } else {
            (void)cbor_value_text_string_equals(&it, "", &result);
        }
    }

    if (is_string_type(type)) {
        const void *chunkptr = NULL;
        size_t chunklen = 0;
        CborValue next = it;
        (void)_cbor_value_get_string_chunk(&it, &chunkptr, &chunklen, &next);
        (void)_cbor_value_get_string_chunk(&it, &chunkptr, &chunklen, NULL);

        {
            size_t outlen = 0;
            CborValue next1 = it;
            err = _cbor_value_copy_string(&it, NULL, &outlen, &next1);
            if (err == CborNoError || err == CborErrorOutOfMemory) {
                if (outlen < (1U << 20)) {
                    uint8_t *out = (uint8_t *)malloc(outlen + 1);
                    if (out) {
                        size_t small = outlen ? outlen / 2 : 0;
                        CborValue next2 = it;
                        (void)_cbor_value_copy_string(&it, out, &small, &next2);

                        next2 = it;
                        (void)_cbor_value_copy_string(&it, out, &outlen, &next2);
                        free(out);
                    }
                }
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t encbuf1[256];
    uint8_t encbuf2[64];
    uint8_t parsebuf[512];
    CborEncoder encoder;
    CborEncoder container;
    CborEncoder e2;
    CborError err;

    cbor_encoder_init(&encoder, encbuf1, sizeof(encbuf1), 0);
    {
        size_t len1 = Size > sizeof(encbuf1) ? sizeof(encbuf1) : Size;
        (void)cbor_encode_byte_string(&encoder, Data, len1);
    }

    cbor_encoder_init(&encoder, encbuf1, sizeof(encbuf1), 0);
    {
        size_t len2 = Size > sizeof(encbuf1) ? sizeof(encbuf1) : Size;
        (void)cbor_encode_text_string(&encoder, (const char *)Data, len2);
    }

    cbor_encoder_init(&encoder, encbuf2, sizeof(encbuf2), 0);
    {
        char *zstr = (char *)malloc(Size + 1);
        if (zstr) {
            if (Size)
                memcpy(zstr, Data, Size);
            zstr[Size] = '\0';
            (void)cbor_encode_text_stringz(&encoder, zstr);
            free(zstr);
        }
    }

    cbor_encoder_init(&encoder, encbuf1, sizeof(encbuf1), 0);
    err = cbor_encoder_create_array(&encoder, &container, CborIndefiniteLength);
    if (err == CborNoError) {
        size_t split = Size / 2;
        (void)cbor_encode_text_string(&container, (const char *)Data, split);
        (void)cbor_encode_text_string(&container, (const char *)Data + split, Size - split);
        (void)cbor_encoder_close_container(&encoder, &container);
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            if (Size)
                fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    fuzz_parsed_value(Data, Size, Data, Size);

    cbor_encoder_init(&e2, parsebuf, sizeof(parsebuf), 0);
    {
        size_t len = Size > 200 ? 200 : Size;
        (void)cbor_encode_text_string(&e2, (const char *)Data, len);
        fuzz_parsed_value(parsebuf, cbor_encoder_get_buffer_size(&e2, parsebuf), Data, len);
    }

    cbor_encoder_init(&e2, parsebuf, sizeof(parsebuf), 0);
    {
        size_t len = Size > 200 ? 200 : Size;
        (void)cbor_encode_byte_string(&e2, Data, len);
        fuzz_parsed_value(parsebuf, cbor_encoder_get_buffer_size(&e2, parsebuf), Data, len);
    }

    return 0;
}