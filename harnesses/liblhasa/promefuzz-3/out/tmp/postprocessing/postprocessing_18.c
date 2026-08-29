#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_reader.h"
#include "lha_decoder.h"
#include "lha_input_stream.h"

static void progress_cb(unsigned int current,
                        unsigned int total,
                        void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }

    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = NULL;
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    LHAFileHeader *header;
    volatile unsigned int progress_sink = 0;
    char outname[64];
    unsigned char buf[4096];
    size_t n;
    int i;

    write_dummy_file(Data, Size);

    fp = fopen("./dummy_file", "rb");
    if (fp == NULL) {
        return 0;
    }

    stream = lha_input_stream_from_FILE(fp);
    if (stream == NULL) {
        fclose(fp);
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Size % 4));
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) ((Size >> 2) % 4));

    for (i = 0; i < 32; ++i) {
        header = lha_reader_next_file(reader);
        if (header == NULL) {
            break;
        }

        (void) lha_reader_current_is_fake(reader);

        if ((Size + (size_t) i) % 3 == 0) {
            (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
        } else if ((Size + (size_t) i) % 3 == 1) {
            (void) lha_reader_check(reader, NULL, NULL);
        }

        if ((Size + (size_t) i) % 2 == 0) {
            n = lha_reader_read(reader, buf, sizeof(buf));
            if (n > 0) {
                progress_sink ^= (unsigned int) buf[0];
            }

            n = lha_reader_read(reader, buf, Size % sizeof(buf));
            if (n > 0) {
                progress_sink ^= (unsigned int) buf[n - 1];
            }
        } else {
            n = lha_reader_read(reader, buf, 0);
            progress_sink ^= (unsigned int) n;
        }

        snprintf(outname, sizeof(outname), "./dummy_out_%d", i);
        if (((Size + (size_t) i) & 1) != 0) {
            (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
        } else {
            (void) lha_reader_extract(reader, outname, NULL, NULL);
        }

        (void) lha_reader_current_is_fake(reader);
    }

    lha_reader_free(reader);
    return 0;
}