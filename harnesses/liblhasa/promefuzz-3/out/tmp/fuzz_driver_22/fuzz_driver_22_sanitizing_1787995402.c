#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int sink = current ^ total;
    if (callback_data != NULL) {
        sink ^= *(const uint8_t *) callback_data;
    }
    (void) sink;
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }

    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    uint8_t cb_data = 0;
    size_t i;
    char outname[64];
    unsigned char read_buf[4096];
    LHAReader *reader = NULL;
    LHADecoder *decoder = NULL;

    write_dummy_file(Data, Size);

    if (Size > 0) {
        cb_data = Data[0];
    }

    /* Exercise lha_decoder_monitor with a standalone decoder object. */
    decoder = (LHADecoder *) calloc(1, sizeof(LHADecoder));
    if (decoder != NULL) {
        if (Size >= 9) {
            decoder->block_size =
                ((unsigned int) Data[1] << 24) ^
                ((unsigned int) Data[2] << 16) ^
                ((unsigned int) Data[3] << 8) ^
                (unsigned int) Data[4];
            decoder->last_block =
                ((unsigned int) Data[5] << 8) ^
                (unsigned int) Data[6];
            decoder->total_blocks =
                ((unsigned int) Data[7] << 8) ^
                (unsigned int) Data[8];
        } else {
            decoder->block_size = 1;
        }

        if (Size >= 25) {
            decoder->stream_pos =
                ((uint64_t) Data[9] << 56) |
                ((uint64_t) Data[10] << 48) |
                ((uint64_t) Data[11] << 40) |
                ((uint64_t) Data[12] << 32) |
                ((uint64_t) Data[13] << 24) |
                ((uint64_t) Data[14] << 16) |
                ((uint64_t) Data[15] << 8) |
                (uint64_t) Data[16];
            decoder->stream_length =
                ((uint64_t) Data[17] << 56) |
                ((uint64_t) Data[18] << 48) |
                ((uint64_t) Data[19] << 40) |
                ((uint64_t) Data[20] << 32) |
                ((uint64_t) Data[21] << 24) |
                ((uint64_t) Data[22] << 16) |
                ((uint64_t) Data[23] << 8) |
                (uint64_t) Data[24];
        } else {
            decoder->stream_length = Size;
            decoder->stream_pos = Size / 2;
        }

        lha_decoder_monitor(decoder, fuzz_progress_callback, &cb_data);
        lha_decoder_monitor(decoder, NULL, NULL);
        free(decoder);
    }

    /* Prefer a real reader if available in the build. */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    reader = lha_reader_new("./dummy_file");
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    if (reader != NULL) {
        LHAReaderDirPolicy policies[3];
        size_t npolicies = 0;

#ifdef LHA_READER_DIR_PLAIN
        policies[npolicies++] = LHA_READER_DIR_PLAIN;
#endif
#ifdef LHA_READER_DIR_END_OF_DIR
        policies[npolicies++] = LHA_READER_DIR_END_OF_DIR;
#endif
#ifdef LHA_READER_DIR_END_OF_FILE
        policies[npolicies++] = LHA_READER_DIR_END_OF_FILE;
#endif
        if (npolicies == 0) {
            policies[npolicies++] = (LHAReaderDirPolicy) 0;
        }

        for (i = 0; i < npolicies; ++i) {
            size_t got;

            lha_reader_set_dir_policy(reader, policies[i]);

            got = lha_reader_read(reader, read_buf,
                                  (Size > 1) ? (size_t)(Data[1] % sizeof(read_buf)) : sizeof(read_buf));
            (void) got;

            got = lha_reader_read(reader, read_buf, sizeof(read_buf));
            (void) got;

            (void) lha_reader_check(reader, fuzz_progress_callback, &cb_data);
            (void) lha_reader_check(reader, NULL, NULL);

            snprintf(outname, sizeof(outname), "./dummy_file.out.%zu", i);
            (void) lha_reader_extract(reader, outname, fuzz_progress_callback, &cb_data);
            (void) lha_reader_extract(reader, outname, NULL, NULL);
        }

        lha_reader_free(reader);
    }

    return 0;
}