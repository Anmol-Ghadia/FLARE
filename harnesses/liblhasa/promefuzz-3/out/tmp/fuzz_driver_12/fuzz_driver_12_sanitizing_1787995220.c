#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void progress_cb(unsigned int current, unsigned int total, void *user_data) {
    volatile unsigned int *sink = (volatile unsigned int *) user_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    LHAReader *reader = NULL;
    LHAFileHeader *hdr;
    volatile unsigned int progress_sink = 0;
    uint8_t small_buf[64];
    uint8_t medium_buf[1024];
    size_t pos = 0;
    int iterations = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        remove("./dummy_file");
        return 0;
    }

    while (iterations < 32) {
        int action;

        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        action = (Size > 0) ? (Data[pos % Size] % 5) : (iterations % 5);
        if (Size > 0) {
            pos++;
        }

        switch (action) {
            case 0: {
                size_t chunk1 = lha_reader_read(reader, small_buf, sizeof(small_buf));
                size_t chunk2 = lha_reader_read(reader, medium_buf, sizeof(medium_buf));
                progress_sink ^= (unsigned int) chunk1;
                progress_sink ^= (unsigned int) chunk2;
                break;
            }

            case 1:
                (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
                break;

            case 2: {
                char outname[64];
                snprintf(outname, sizeof(outname), "./dummy_file.out.%d", iterations);
                (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
                remove(outname);
                break;
            }

            case 3: {
                size_t chunk = lha_reader_read(reader, medium_buf, sizeof(medium_buf));
                progress_sink ^= (unsigned int) chunk;
                (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
                break;
            }

            case 4:
            default: {
                char outname[64];
                size_t chunk;
                snprintf(outname, sizeof(outname), "./dummy_file.out.%d", iterations);
                (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
                remove(outname);
                chunk = lha_reader_read(reader, small_buf, sizeof(small_buf));
                progress_sink ^= (unsigned int) chunk;
                (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
                break;
            }
        }

        iterations++;
    }

    lha_reader_free(reader);
    remove("./dummy_file");
    return 0;
}