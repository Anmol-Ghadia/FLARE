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

    CborEncoder encoder;
    int flags = (Size > 0) ? (int)Data[0] : 0;
    cbor_encoder_init(&encoder, buffer, bufsize, flags);

    size_t offset = (Size > 0) ? 1 : 0;

    if (offset <= Size) {
        size_t len1 = Size - offset;
        size_t use1 = 0;
        if (len1 > 0)
            use1 = (Size > 1) ? ((size_t)Data[0] % (len1 + 1)) : len1;
        (void)cbor_encode_text_string(&encoder, (const char *)(Data + offset), use1);
    }

    if (offset < Size) {
        size_t rem = Size - offset;
        size_t rawlen = (size_t)Data[offset] % (rem + 1);
        (void)cbor_encode_raw(&encoder, Data + offset, rawlen);
        offset += rawlen;
    } else {
        (void)cbor_encode_raw(&encoder, Data, 0);
    }

    (void)_cbor_encoder_get_buffer_pointer(&encoder);
    (void)cbor_encoder_get_buffer_size(&encoder, buffer);

    {
        CborEncoder parent2;
        cbor_encoder_init(&parent2, buffer, bufsize, flags ^ 0x55);
        (void)cbor_encoder_close_container(&parent2, &encoder);
        (void)_cbor_encoder_get_buffer_pointer(&parent2);
        (void)cbor_encoder_get_buffer_size(&parent2, buffer);
    }

    {
        CborEncoder encoder2;
        cbor_encoder_init(&encoder2, buffer, bufsize, flags ^ 0xaa);

        if (Size > 2) {
            size_t tlen = (size_t)Data[1] % (Size - 1);
            (void)cbor_encode_text_string(&encoder2, (const char *)(Data + 1), tlen);
        } else {
            (void)cbor_encode_text_string(&encoder2, "", 0);
        }

        if (Size > 3) {
            size_t rawoff = 2;
            if (rawoff < Size) {
                size_t rawrem = Size - rawoff;
                size_t rawlen2 = (size_t)Data[2] % (rawrem + 1);
                (void)cbor_encode_raw(&encoder2, Data + rawoff, rawlen2);
            }
        } else {
            (void)cbor_encode_raw(&encoder2, Data, 0);
        }

        (void)_cbor_encoder_get_buffer_pointer(&encoder2);
        (void)cbor_encoder_get_buffer_size(&encoder2, buffer);
        (void)cbor_encoder_close_container(&encoder, &encoder2);
    }

    free(buffer);
    return 0;
}