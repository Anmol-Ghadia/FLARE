#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t bufsize = Size + 64;
    uint8_t *buffer = (uint8_t *)malloc(bufsize ? bufsize : 1);
    if (!buffer)
        return 0;
    memset(buffer, 0, bufsize);

    {
        CborEncoder encoder;
        int flags = 0;
        size_t offset = 0;
        size_t len1 = 0;
        size_t rawlen = 0;
        uint8_t *ptr;

        cbor_encoder_init(&encoder, buffer, bufsize, flags);

        if (Size > 0) {
            flags = (int)Data[0];
            offset = 1;
            cbor_encoder_init(&encoder, buffer, bufsize, flags);
        }

        if (offset <= Size) {
            size_t rem = Size - offset;
            if (rem > 0)
                len1 = (Size > 1) ? ((size_t)Data[0] % (rem + 1)) : rem;
            (void)cbor_encode_text_string(&encoder, (const char *)(Data + offset), len1);
        }

        ptr = _cbor_encoder_get_buffer_pointer(&encoder);
        if (ptr >= buffer && ptr <= buffer + bufsize) {
            size_t used = (size_t)(ptr - buffer);
            (void)used;
        }
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);

        if (offset < Size) {
            size_t rem2 = Size - offset;
            rawlen = (size_t)Data[offset] % (rem2 + 1);
            (void)cbor_encode_raw(&encoder, Data + offset, rawlen);
        } else {
            (void)cbor_encode_raw(&encoder, Data, 0);
        }

        ptr = _cbor_encoder_get_buffer_pointer(&encoder);
        if (ptr >= buffer && ptr <= buffer + bufsize) {
            size_t used2 = (size_t)(ptr - buffer);
            (void)used2;
        }
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);

        {
            CborEncoder parent2;
            cbor_encoder_init(&parent2, buffer, bufsize, 0);
            (void)cbor_encoder_close_container(&parent2, &encoder);

            ptr = _cbor_encoder_get_buffer_pointer(&parent2);
            if (ptr >= buffer && ptr <= buffer + bufsize) {
                size_t used3 = (size_t)(ptr - buffer);
                (void)used3;
            }
            (void)cbor_encoder_get_buffer_size(&parent2, buffer);
        }
    }

    {
        CborEncoder encoder2;
        uint8_t *ptr2;

        cbor_encoder_init(&encoder2, buffer, bufsize, 0);

        if (Size > 1) {
            size_t tlen = (size_t)Data[0] % Size;
            (void)cbor_encode_text_string(&encoder2, (const char *)Data, tlen);
        } else {
            (void)cbor_encode_text_string(&encoder2, "", 0);
        }

        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder2);
        if (ptr2 >= buffer && ptr2 <= buffer + bufsize) {
            size_t used4 = (size_t)(ptr2 - buffer);
            (void)used4;
        }
        (void)cbor_encoder_get_buffer_size(&encoder2, buffer);

        if (Size > 0) {
            size_t rawlen2 = (size_t)Data[Size - 1] % (Size + 1);
            (void)cbor_encode_raw(&encoder2, Data, rawlen2);
        } else {
            (void)cbor_encode_raw(&encoder2, Data, 0);
        }

        ptr2 = _cbor_encoder_get_buffer_pointer(&encoder2);
        if (ptr2 >= buffer && ptr2 <= buffer + bufsize) {
            size_t used5 = (size_t)(ptr2 - buffer);
            (void)used5;
        }
        (void)cbor_encoder_get_buffer_size(&encoder2, buffer);

        {
            CborEncoder parent3;
            cbor_encoder_init(&parent3, buffer, bufsize, 0);
            (void)cbor_encoder_close_container(&parent3, &encoder2);

            ptr2 = _cbor_encoder_get_buffer_pointer(&parent3);
            if (ptr2 >= buffer && ptr2 <= buffer + bufsize) {
                size_t used6 = (size_t)(ptr2 - buffer);
                (void)used6;
            }
            (void)cbor_encoder_get_buffer_size(&parent3, buffer);
        }
    }

    free(buffer);
    return 0;
}