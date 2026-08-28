#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbor.h"

static size_t read_size_value(const uint8_t *data, size_t size, size_t *offset) {
    size_t v = 0;
    size_t i;
    for (i = 0; i < sizeof(size_t) && *offset < size; ++i) {
        v = (v << 8) | data[*offset];
        (*offset)++;
    }
    return v;
}

static uint64_t read_u64_value(const uint8_t *data, size_t size, size_t *offset) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8 && *offset < size; ++i) {
        v = (v << 8) | data[*offset];
        (*offset)++;
    }
    return v;
}

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType type) {
    FILE *fp = (FILE *)token;
    (void)type;
    if (!fp)
        return CborErrorIO;
    if (len == 0)
        return CborNoError;
    if (fwrite(data, 1, len, fp) != len)
        return CborErrorIO;
    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    size_t buf_size;
    int flags;
    uint8_t *buffer;
    CborEncoder enc;
    CborEncoder writer_enc;
    CborError err;
    size_t i, iterations, writer_iterations;
    FILE *fp = NULL;

    buf_size = (Size > 0) ? (read_size_value(Data, Size, &off) % 256) : 0;
    flags = (Size > 0) ? (int)(read_size_value(Data, Size, &off) & 0xffff) : 0;

    buffer = (uint8_t *)malloc(buf_size ? buf_size : 1);
    if (!buffer)
        return 0;

    cbor_encoder_init(&enc, buffer, buf_size, flags);

    iterations = (Size > off) ? ((Data[off++] % 32) + 1) : 1;
    for (i = 0; i < iterations; ++i) {
        uint64_t v = (off < Size) ? read_u64_value(Data, Size, &off) : (uint64_t)i;
        if ((i & 1) == 0)
            err = cbor_encode_uint(&enc, v);
        else
            err = cbor_encode_negative_int(&enc, v);
        (void)err;
        (void)cbor_encoder_get_extra_bytes_needed(&enc);
    }

    {
        CborEncoder child = enc;
        err = cbor_encoder_close_container(&enc, &child);
        (void)err;
        (void)cbor_encoder_get_extra_bytes_needed(&enc);
    }

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (off < Size)
            fwrite(Data + off, 1, Size - off, fp);
        fclose(fp);
    }

    fp = fopen("./dummy_file", "ab+");
    if (fp) {
        cbor_encoder_init_writer(&writer_enc, fuzz_writer, fp);

        writer_iterations = (Size > off) ? ((Data[off++] % 32) + 1) : 1;
        for (i = 0; i < writer_iterations; ++i) {
            uint64_t v = (off < Size) ? read_u64_value(Data, Size, &off) : (uint64_t)(i * 3);
            if (off < Size && (Data[off++] & 1))
                err = cbor_encode_uint(&writer_enc, v);
            else
                err = cbor_encode_negative_int(&writer_enc, v);
            (void)err;
            (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
        }

        {
            CborEncoder child = writer_enc;
            err = cbor_encoder_close_container(&writer_enc, &child);
            (void)err;
            (void)cbor_encoder_get_extra_bytes_needed(&writer_enc);
        }

        fclose(fp);
    }

    free(buffer);
    return 0;
}