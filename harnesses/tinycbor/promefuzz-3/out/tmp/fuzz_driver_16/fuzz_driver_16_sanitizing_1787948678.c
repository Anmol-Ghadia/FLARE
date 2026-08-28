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

static int fuzz_writer(void *token, const void *data, size_t len, int flags)
{
    (void)flags;
    if (!token)
        return 0;

    FILE *fp = (FILE *)token;
    if (len == 0)
        return 0;

    if (data == NULL) {
        static const uint8_t zero = 0;
        return fwrite(&zero, 1, 1, fp) == 1 ? 0 : -1;
    }

    return fwrite(data, 1, len, fp) == len ? 0 : -1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    if (!Data)
        return 0;

    size_t buf_size = Size > 0 ? (size_t)Data[0] : 0;
    uint8_t *buffer = NULL;
    if (buf_size > 0) {
        buffer = (uint8_t *)malloc(buf_size);
        if (!buffer)
            return 0;
        if (Size > 1) {
            size_t copy = Size - 1 < buf_size ? Size - 1 : buf_size;
            memcpy(buffer, Data + 1, copy);
        }
    }

    int flags = 0;
    if (Size >= 5) {
        flags = (int)((unsigned)Data[1] |
                      ((unsigned)Data[2] << 8) |
                      ((unsigned)Data[3] << 16) |
                      ((unsigned)Data[4] << 24));
    } else if (Size >= 2) {
        flags = (int)Data[1];
    }

    CborEncoder enc;
    cbor_encoder_init(&enc, buffer, buf_size, flags);

    size_t offset = Size > 5 ? 5 : Size;
    while (offset < Size) {
        uint8_t op = Data[offset++];
        uint64_t value = 0;
        int i;
        for (i = 0; i < 8 && offset < Size; i++, offset++)
            value |= ((uint64_t)Data[offset]) << (8 * i);

        if (op & 1) {
            (void)cbor_encode_uint(&enc, value);
        } else {
            (void)cbor_encode_negative_int(&enc, value);
        }

        (void)cbor_encoder_get_extra_bytes_needed(&enc);
    }

    (void)cbor_encoder_close_container(&enc, &enc);
    (void)cbor_encoder_get_extra_bytes_needed(&enc);

    FILE *fp = fopen("./dummy_file", "wb+");
    if (fp) {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        (void)fflush(fp);
        (void)fseek(fp, 0, SEEK_SET);

        CborEncoder writer_enc;
        cbor_encoder_init_writer(&writer_enc, fuzz_writer, fp);

        offset = 0;
        while (offset < Size) {
            uint8_t op = Data[offset++];
            uint64_t value = 0;
            int i;
            for (i = 0; i < 8 && offset < Size; i++, offset++)
                value |= ((uint64_t)Data[offset]) << (8 * i);

            if (op & 2) {
                (void)cbor_encode_uint(&writer_enc, value);
            } else {
                (void)cbor_encode_negative_int(&writer_enc, value);
            }

            (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
        }

        (void)cbor_encoder_close_container(&writer_enc, &writer_enc);
        (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
        fclose(fp);
    }

    free(buffer);
    return 0;
}