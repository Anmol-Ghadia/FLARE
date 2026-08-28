#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cbor.h"

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType appendType)
{
    (void)appendType;

    if (!token)
        return CborErrorIO;
    if (len == 0)
        return CborNoError;
    if (!data)
        return CborErrorIO;

    return fwrite(data, 1, len, (FILE *)token) == len ? CborNoError : CborErrorIO;
}

static uint64_t read_u64(const uint8_t *data, size_t size, size_t *offset)
{
    uint64_t value = 0;
    size_t i;

    for (i = 0; i < 8 && *offset < size; i++, (*offset)++)
        value |= ((uint64_t)data[*offset]) << (8 * i);

    return value;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    size_t buf_size = 0;
    int flags = 0;
    uint8_t *buffer = NULL;
    CborEncoder enc;

    if (Size > 0)
        buf_size = (size_t)Data[offset++];

    if (Size - offset >= 4) {
        flags = (int)((unsigned)Data[offset] |
                      ((unsigned)Data[offset + 1] << 8) |
                      ((unsigned)Data[offset + 2] << 16) |
                      ((unsigned)Data[offset + 3] << 24));
        offset += 4;
    } else if (offset < Size) {
        flags = (int)Data[offset++];
    }

    if (buf_size != 0) {
        buffer = (uint8_t *)malloc(buf_size);
        if (!buffer)
            return 0;
        memset(buffer, 0, buf_size);
    }

    cbor_encoder_init(&enc, buffer, buf_size, flags);

    while (offset < Size) {
        uint8_t op = Data[offset++];
        uint64_t value = read_u64(Data, Size, &offset);

        if (op & 1)
            (void)cbor_encode_uint(&enc, value);
        else
            (void)cbor_encode_negative_int(&enc, value);

        (void)cbor_encoder_get_extra_bytes_needed(&enc);
    }

    (void)cbor_encoder_get_extra_bytes_needed(&enc);

    {
        FILE *fp = fopen("./dummy_file", "wb+");
        if (fp) {
            CborEncoder writer_enc;
            size_t writer_offset = 0;

            if (Size > 0)
                (void)fwrite(Data, 1, Size, fp);
            (void)fflush(fp);
            (void)fseek(fp, 0, SEEK_SET);

            cbor_encoder_init_writer(&writer_enc, fuzz_writer, fp);

            while (writer_offset < Size) {
                uint8_t op = Data[writer_offset++];
                uint64_t value = read_u64(Data, Size, &writer_offset);

                if (op & 2)
                    (void)cbor_encode_uint(&writer_enc, value);
                else
                    (void)cbor_encode_negative_int(&writer_enc, value);

                (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
            }

            (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
            fclose(fp);
        }
    }

    {
        CborEncoder parent;
        CborEncoder child;
        uint8_t localbuf[32];

        cbor_encoder_init(&parent, localbuf, sizeof(localbuf), 0);
        cbor_encoder_create_array(&parent, &child, 1);
        (void)cbor_encode_uint(&child, 0);
        (void)cbor_encoder_close_container(&parent, &child);
        (void)cbor_encoder_get_extra_bytes_needed(&parent);
    }

    free(buffer);
    return 0;
}