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
    char outname[64];
    size_t i, max_files, read_sz;
    uint8_t buf[4096];

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
        return 0;
    }

    max_files = (Size > 0) ? ((size_t)Data[0] % 16u + 1u) : 4u;

    for (i = 0; i < max_files; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        if ((i & 1u) == 0u) {
            (void) lha_reader_check(reader, progress_cb, (void *)&progress_sink);
        } else {
            (void) lha_reader_check(reader, NULL, NULL);
        }

        if ((i & 2u) == 0u) {
            read_sz = sizeof(buf);
            if (Size > (i + 1u)) {
                read_sz = (size_t)(Data[i + 1u] % (sizeof(buf) + 1u));
            }

            if (read_sz > 0) {
                (void) lha_reader_read(reader, buf, read_sz);
            }
            (void) lha_reader_read(reader, buf, sizeof(buf));
        } else {
            (void) lha_reader_read(reader, buf, 1);
            (void) lha_reader_read(reader, buf, 0);
        }

        snprintf(outname, sizeof(outname), "./dummy_file_out_%zu", i);
        if ((i & 4u) == 0u) {
            (void) lha_reader_extract(reader, outname, progress_cb, (void *)&progress_sink);
        } else {
            (void) lha_reader_extract(reader, outname, NULL, NULL);
        }

        if (reader->decoder != NULL) {
            lha_decoder_monitor(reader->decoder, progress_cb, (void *)&progress_sink);
            lha_decoder_monitor(reader->decoder, NULL, NULL);
        }
        if (reader->inner_decoder != NULL && reader->inner_decoder != reader->decoder) {
            lha_decoder_monitor(reader->inner_decoder, progress_cb, (void *)&progress_sink);
        }
    }

    lha_reader_free(reader);
    return 0;
}