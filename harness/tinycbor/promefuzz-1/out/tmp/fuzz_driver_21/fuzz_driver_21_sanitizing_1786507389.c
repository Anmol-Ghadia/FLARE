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

static CborError fuzz_writer(void *token, const void *data, size_t len, CborEncoderAppendType type)
{
    (void)type;
    FILE *fp = (FILE *)token;
    if (!fp)
        return CborErrorIO;

    if (len == 0)
        return CborNoError;

    if (!data)
        return CborErrorIO;

    if (fwrite(data, 1, len, fp) != len)
        return CborErrorIO;

    return CborNoError;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    if (!Data)
        return 0;

    uint8_t *outbuf = NULL;
    size_t outcap = 0;
    size_t raw_off = 0;
    size_t raw_len = 0;
    int mode = 0;

    if (Size > 0)
        mode = Data[0] % 4;
    if (Size > 1)
        outcap = (size_t)Data[1];
    if (Size > 2)
        raw_off = (size_t)Data[2] % (Size + 1);
    if (Size > 3)
        raw_len = (size_t)Data[3];

    if (raw_off > Size)
        raw_off = Size;
    if (raw_len > Size - raw_off)
        raw_len = Size - raw_off;

    if (outcap > 0) {
        outbuf = (uint8_t *)malloc(outcap);
        if (!outbuf)
            return 0;
        memset(outbuf, 0, outcap);
    }

    if (mode == 0 || mode == 1) {
        CborEncoder enc;
        cbor_encoder_init(&enc, outbuf, outcap, 0);

        (void)_cbor_encoder_get_buffer_pointer(&enc);
        (void)cbor_encoder_get_extra_bytes_needed(&enc);

        CborError err = cbor_encode_raw(&enc, Data + raw_off, raw_len);
        if (err == CborNoError || err == CborErrorOutOfMemory || err == CborErrorDataTooLarge) {
            (void)_cbor_encoder_get_buffer_pointer(&enc);
            (void)cbor_encoder_get_extra_bytes_needed(&enc);
            if (outbuf)
                (void)cbor_encoder_get_buffer_size(&enc, outbuf);
        }

        if (mode == 1) {
            size_t raw_off2 = raw_off;
            size_t raw_len2 = raw_len;

            if (Size > 4)
                raw_off2 = (size_t)Data[4] % (Size + 1);
            if (raw_off2 > Size)
                raw_off2 = Size;
            if (Size > 5)
                raw_len2 = (size_t)Data[5];
            if (raw_len2 > Size - raw_off2)
                raw_len2 = Size - raw_off2;

            if (err == CborNoError) {
                (void)cbor_encode_raw(&enc, Data + raw_off2, raw_len2);
                (void)_cbor_encoder_get_buffer_pointer(&enc);
                (void)cbor_encoder_get_extra_bytes_needed(&enc);
                if (outbuf)
                    (void)cbor_encoder_get_buffer_size(&enc, outbuf);
            }
        }
    } else {
        CborEncoder enc;
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp) {
            cbor_encoder_init_writer(&enc, fuzz_writer, fp);

            (void)_cbor_encoder_get_buffer_pointer(&enc);
            (void)cbor_encoder_get_extra_bytes_needed(&enc);

            CborError err = cbor_encode_raw(&enc, Data + raw_off, raw_len);
            (void)err;

            (void)_cbor_encoder_get_buffer_pointer(&enc);
            (void)cbor_encoder_get_extra_bytes_needed(&enc);

            if (mode == 3) {
                size_t split = raw_len;
                if (Size > 4)
                    split = (size_t)Data[4] % (raw_len + 1);

                (void)cbor_encode_raw(&enc, Data + raw_off, split);
                (void)cbor_encode_raw(&enc, Data + raw_off + split, raw_len - split);
                (void)_cbor_encoder_get_buffer_pointer(&enc);
                (void)cbor_encoder_get_extra_bytes_needed(&enc);
            }

            fclose(fp);
        }
    }

    free(outbuf);
    return 0;
}