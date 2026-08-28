#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t encbuf1[256];
    uint8_t encbuf2[64];
    CborEncoder encoder;
    CborEncoder container;
    CborError err;

    cbor_encoder_init(&encoder, encbuf1, sizeof(encbuf1), 0);

    if (Size > 0) {
        size_t len1 = Size;
        if (len1 > sizeof(encbuf1))
            len1 = sizeof(encbuf1);
        (void)cbor_encode_byte_string(&encoder, Data, len1);
    } else {
        (void)cbor_encode_byte_string(&encoder, Data, 0);
    }

    cbor_encoder_init(&encoder, encbuf1, sizeof(encbuf1), 0);
    {
        size_t len2 = Size;
        if (len2 > sizeof(encbuf1))
            len2 = sizeof(encbuf1);
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

    {
        CborParser parser;
        CborValue it;
        err = cbor_parser_init(Data, Size, 0, &parser, &it);
        if (err == CborNoError) {
            bool result = false;
            char *cmp = (char *)malloc(Size + 1);
            if (cmp) {
                if (Size)
                    memcpy(cmp, Data, Size);
                cmp[Size] = '\0';
                (void)cbor_value_text_string_equals(&it, cmp, &result);
                free(cmp);
            } else {
                (void)cbor_value_text_string_equals(&it, "", &result);
            }

            {
                const void *chunkptr = NULL;
                size_t chunklen = 0;
                CborValue next = it;
                (void)_cbor_value_get_string_chunk(&it, &chunkptr, &chunklen, &next);
                (void)_cbor_value_get_string_chunk(&it, &chunkptr, &chunklen, NULL);
            }

            {
                size_t outlen = 0;
                CborValue next = it;
                (void)_cbor_value_copy_string(&it, NULL, &outlen, &next);

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

    {
        uint8_t parsebuf[512];
        CborEncoder e2;
        size_t len = Size > 200 ? 200 : Size;
        cbor_encoder_init(&e2, parsebuf, sizeof(parsebuf), 0);
        (void)cbor_encode_text_string(&e2, (const char *)Data, len);

        {
            size_t used = cbor_encoder_get_buffer_size(&e2, parsebuf);
            CborParser parser;
            CborValue it;
            err = cbor_parser_init(parsebuf, used, 0, &parser, &it);
            if (err == CborNoError) {
                bool result = false;
                char *cmp = (char *)malloc(len + 1);
                if (cmp) {
                    if (len)
                        memcpy(cmp, Data, len);
                    cmp[len] = '\0';
                    (void)cbor_value_text_string_equals(&it, cmp, &result);

                    {
                        const void *chunkptr = NULL;
                        size_t chunklen = 0;
                        CborValue next = it;
                        (void)_cbor_value_get_string_chunk(&it, &chunkptr, &chunklen, &next);
                    }

                    {
                        size_t buflen = len + 8;
                        char *out = (char *)malloc(buflen ? buflen : 1);
                        if (out) {
                            CborValue next = it;
                            (void)_cbor_value_copy_string(&it, out, &buflen, &next);
                            free(out);
                        }
                    }

                    free(cmp);
                }
            }
        }
    }

    return 0;
}