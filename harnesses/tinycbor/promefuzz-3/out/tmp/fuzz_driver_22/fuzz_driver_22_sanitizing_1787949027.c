#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

    CborEncoder encoder;
    int flags = 0;
    if (Size > 0)
        flags = (int)Data[0];
    cbor_encoder_init(&encoder, buffer, bufsize, flags);

    size_t offset = Size > 0 ? 1 : 0;

    if (offset <= Size) {
        size_t len1 = Size - offset;
        if (len1 > 0) {
            size_t use1 = len1;
            if (Size > 1)
                use1 = (size_t)Data[offset - 1] % (len1 + 1);
            (void)cbor_encode_text_string(&encoder, (const char *)Data + offset, use1);
        } else {
            (void)cbor_encode_text_string(&encoder, "", 0);
        }
    }

    if (offset < Size) {
        size_t rem = Size - offset;
        size_t rawlen = rem;
        if (rem > 0)
            rawlen = (size_t)Data[offset] % (rem + 1);
        (void)cbor_encode_raw(&encoder, Data + offset, rawlen);
        offset += rawlen;
    } else {
        (void)cbor_encode_raw(&encoder, Data, 0);
    }

    (void)_cbor_encoder_get_buffer_pointer(&encoder);
    (void)cbor_encoder_get_buffer_size(&encoder, buffer);

    CborEncoder fake_container1 = encoder;
    if (Size > 2) {
        fake_container1.remaining = (size_t)(Data[1] % 4);
        fake_container1.flags ^= (int)Data[2];
        if (fake_container1.end && fake_container1.data.ptr) {
            ptrdiff_t span = fake_container1.end - buffer;
            if (span > 0) {
                size_t pos = (size_t)Data[1] % ((size_t)span + 1);
                fake_container1.data.ptr = buffer + (pos <= (size_t)span ? pos : (size_t)span);
            }
        }
        (void)cbor_encoder_close_container(&encoder, &fake_container1);
    }

    CborEncoder fake_parent;
    cbor_encoder_init(&fake_parent, buffer, bufsize / 2, flags ^ 0x55);
    if (Size > 3) {
        (void)cbor_encode_raw(&fake_parent, Data, Data[3] % (Size + 1));
    }
    (void)cbor_encoder_close_container(&fake_parent, &encoder);

    CborEncoder fake_container2;
    cbor_encoder_init(&fake_container2, buffer, bufsize, flags ^ 0xaa);
    if (Size > 4) {
        size_t tlen = (size_t)Data[4] % (Size + 1);
        (void)cbor_encode_text_string(&fake_container2, (const char *)Data, tlen);
    }
    fake_container2.remaining = (Size > 5) ? (size_t)Data[5] : 0;
    (void)cbor_encoder_close_container(&encoder, &fake_container2);

    (void)_cbor_encoder_get_buffer_pointer(&fake_parent);
    (void)cbor_encoder_get_buffer_size(&fake_parent, buffer);
    (void)_cbor_encoder_get_buffer_pointer(&fake_container2);
    (void)cbor_encoder_get_buffer_size(&fake_container2, buffer);

    free(buffer);
    return 0;
}