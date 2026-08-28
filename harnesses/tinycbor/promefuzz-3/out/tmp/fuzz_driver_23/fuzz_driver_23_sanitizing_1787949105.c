#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    size_t bufsize = Size + 32;
    uint8_t *buffer = (uint8_t *)malloc(bufsize ? bufsize : 1);
    if (!buffer)
        return 0;

    memset(buffer, 0, bufsize);

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, bufsize, (Size > 0) ? (int)(Data[0] & 0x7) : 0);

    (void)_cbor_encoder_get_buffer_pointer(&encoder);
    (void)cbor_encoder_get_buffer_size(&encoder, buffer);

    if (Size == 0) {
        free(buffer);
        return 0;
    }

    size_t offset = 1;

    if (offset <= Size) {
        size_t len1 = (Size - offset > 0) ? (size_t)(Data[0] % (Size - offset + 1)) : 0;
        if (offset + len1 > Size)
            len1 = Size - offset;
        (void)cbor_encode_text_string(&encoder, (const char *)Data + offset, len1);
        (void)_cbor_encoder_get_buffer_pointer(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);
        offset += len1;
    }

    if (offset <= Size) {
        size_t len2 = Size - offset;
        (void)cbor_encode_raw(&encoder, Data + offset, len2);
        (void)_cbor_encoder_get_buffer_pointer(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);
    }

    if (Size > 2) {
        CborEncoder parent;
        CborEncoder child;
        size_t bufsize2 = Size + 16;
        uint8_t *buffer2 = (uint8_t *)malloc(bufsize2 ? bufsize2 : 1);
        if (buffer2) {
            memset(buffer2, 0, bufsize2);
            cbor_encoder_init(&parent, buffer2, bufsize2, (int)(Data[1] & 0x3));
            cbor_encoder_init(&child, buffer2, bufsize2, (int)(Data[2] & 0x3));

            (void)cbor_encode_text_string(&child, (const char *)Data, Size > 4 ? 4 : Size);
            (void)cbor_encode_raw(&child, Data, Size > 8 ? 8 : Size);
            (void)_cbor_encoder_get_buffer_pointer(&child);
            (void)cbor_encoder_get_buffer_size(&child, buffer2);

            /* Intentionally avoid invalid parent/container pairing misuse. */
            free(buffer2);
        }
    }

    free(buffer);
    return 0;
}