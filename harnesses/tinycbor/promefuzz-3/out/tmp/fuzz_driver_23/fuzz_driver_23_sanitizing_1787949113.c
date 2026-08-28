#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cbor.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size != 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    size_t bufsize = Size + 32;
    if (bufsize == 0)
        bufsize = 32;

    uint8_t *buffer = (uint8_t *)malloc(bufsize);
    if (!buffer)
        return 0;
    memset(buffer, 0, bufsize);

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, bufsize, 0);

    (void)_cbor_encoder_get_buffer_pointer(&encoder);
    (void)cbor_encoder_get_buffer_size(&encoder, buffer);

    if (Size > 0) {
        size_t split = (size_t)Data[0] % (Size + 1);
        size_t text_len = split < Size ? split : Size;
        size_t raw_off = text_len;
        size_t raw_len = Size - raw_off;

        (void)cbor_encode_text_string(&encoder, (const char *)Data, text_len);
        (void)_cbor_encoder_get_buffer_pointer(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);

        if (raw_len > 0)
            (void)cbor_encode_raw(&encoder, Data + raw_off, raw_len);
        else
            (void)cbor_encode_raw(&encoder, Data, 0);

        (void)_cbor_encoder_get_buffer_pointer(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);
    } else {
        (void)cbor_encode_text_string(&encoder, "", 0);
        (void)cbor_encode_raw(&encoder, (const uint8_t *)"", 0);
        (void)_cbor_encoder_get_buffer_pointer(&encoder);
        (void)cbor_encoder_get_buffer_size(&encoder, buffer);
    }

    free(buffer);
    return 0;
}