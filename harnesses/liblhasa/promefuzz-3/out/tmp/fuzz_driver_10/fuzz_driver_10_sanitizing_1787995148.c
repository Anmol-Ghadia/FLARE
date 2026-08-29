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

static void progress_cb(unsigned int current, unsigned int total, void *user_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) user_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    LHAReader *reader = NULL;
    volatile unsigned int progress_sink = 0;
    char outname[64];
    uint8_t buf[4096];
    size_t n;
    size_t offset = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        return 0;
    }

    if (Size > 0) {
        snprintf(outname, sizeof(outname), "./dummy_out_%u",
                 (unsigned int) Data[0]);
    } else {
        snprintf(outname, sizeof(outname), "./dummy_out");
    }

    if (reader->decoder != NULL) {
        lha_decoder_monitor(reader->decoder, progress_cb, (void *) &progress_sink);
        lha_decoder_monitor(reader->decoder, NULL, NULL);
        lha_decoder_monitor(reader->decoder, progress_cb, (void *) &progress_sink);
    }

    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
    (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);

    while (offset < Size + 1) {
        size_t want = 1;
        if (offset < Size) {
            want = (size_t) (Data[offset] % sizeof(buf)) + 1;
        }

        n = lha_reader_read(reader, buf, want);
        if (reader->decoder != NULL) {
            lha_decoder_monitor(reader->decoder, progress_cb, (void *) &progress_sink);
        }

        (void) lha_reader_current_is_fake(reader);

        if (n == 0) {
            break;
        }

        offset += n;

        if ((offset & 1) == 0) {
            (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
        } else {
            (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
        }
    }

    if (reader->decoder != NULL) {
        lha_decoder_monitor(reader->decoder, progress_cb, (void *) &progress_sink);
    }

    (void) lha_reader_check(reader, NULL, NULL);
    (void) lha_reader_extract(reader, outname, NULL, NULL);
    (void) lha_reader_current_is_fake(reader);

    lha_reader_free(reader);
    return 0;
}