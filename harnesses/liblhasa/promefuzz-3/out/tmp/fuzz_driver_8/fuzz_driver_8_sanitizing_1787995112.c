#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int sink = current ^ total;
    if (callback_data != NULL) {
        volatile uint8_t *p = (volatile uint8_t *) callback_data;
        *p ^= (uint8_t) sink;
    }
}

static void write_dummy_file(const uint8_t *data, size_t size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (size > 0) {
            (void) fwrite(data, 1, size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    LHAReader *reader = NULL;
    LHADecoder *decoder = NULL;
    uint8_t callback_state = 0;
    uint8_t read_buf[256];
    size_t to_read, nread;
    int policy_sel = 0;

    write_dummy_file(Data, Size);

    if (Size >= 1) {
        policy_sel = Data[0] % 4;
    }

    reader = lha_reader_new("./dummy_file");
    if (reader != NULL) {
        switch (policy_sel) {
            case 0:
                lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 0);
                break;
            case 1:
                lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 1);
                break;
            case 2:
                lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 2);
                break;
            default:
                lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 3);
                break;
        }

        to_read = 0;
        if (Size >= 2) {
            to_read = (size_t) Data[1];
            if (to_read > sizeof(read_buf)) {
                to_read = sizeof(read_buf);
            }
        }

        nread = lha_reader_read(reader, read_buf, to_read);
        (void) nread;

        (void) lha_reader_check(reader, fuzz_progress_callback, &callback_state);
        (void) lha_reader_extract(reader, (char *) "./dummy_file", fuzz_progress_callback, &callback_state);

        if (Size >= 3) {
            size_t to_read2 = (size_t) Data[2];
            if (to_read2 > sizeof(read_buf)) {
                to_read2 = sizeof(read_buf);
            }
            nread = lha_reader_read(reader, read_buf, to_read2);
            (void) nread;
        }

        (void) lha_reader_check(reader, NULL, NULL);
        (void) lha_reader_extract(reader, (char *) "./dummy_file", NULL, NULL);

        lha_reader_free(reader);
    }

    decoder = (LHADecoder *) calloc(1, sizeof(LHADecoder));
    if (decoder != NULL) {
        if (Size >= 11) {
            decoder->block_size =
                ((unsigned int) Data[3] << 24) |
                ((unsigned int) Data[4] << 16) |
                ((unsigned int) Data[5] << 8)  |
                ((unsigned int) Data[6]);
            decoder->stream_length =
                ((uint64_t) Data[7] << 24) |
                ((uint64_t) Data[8] << 16) |
                ((uint64_t) Data[9] << 8)  |
                ((uint64_t) Data[10]);
        } else {
            decoder->block_size = 1;
            decoder->stream_length = 0;
        }

        decoder->stream_pos = (Size >= 12) ? Data[11] : 0;
        decoder->last_block = (Size >= 13) ? Data[12] : 0;
        decoder->total_blocks = (Size >= 14) ? Data[13] : 0;

        lha_decoder_monitor(decoder, fuzz_progress_callback, &callback_state);
        lha_decoder_monitor(decoder, NULL, NULL);
        lha_decoder_monitor(decoder, fuzz_progress_callback, NULL);

        free(decoder);
    }

    remove("./dummy_file");
    return 0;
}