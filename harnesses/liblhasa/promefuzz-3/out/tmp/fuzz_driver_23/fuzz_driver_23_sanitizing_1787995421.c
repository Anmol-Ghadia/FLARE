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

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    volatile unsigned int progress_sink = 0;
    FILE *fp;
    LHAReader *reader = NULL;
    char outname[64];
    uint8_t buf[4096];
    size_t n;
    int i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        return 0;
    }

    if (reader->decoder != NULL) {
        lha_decoder_monitor(reader->decoder, fuzz_progress_callback, (void *) &progress_sink);
    }

    for (i = 0; i < 8; ++i) {
        if (!lha_reader_next_file(reader)) {
            break;
        }

        if (reader->decoder != NULL) {
            lha_decoder_monitor(reader->decoder, fuzz_progress_callback, (void *) &progress_sink);
        }

        (void) lha_reader_current_is_fake(reader);

        if ((Size + (size_t) i) % 3 == 0) {
            (void) lha_reader_check(reader, fuzz_progress_callback, (void *) &progress_sink);
        }

        if ((Size + (size_t) i) % 2 == 0) {
            memset(buf, 0, sizeof(buf));
            n = lha_reader_read(reader, buf, sizeof(buf));
            progress_sink ^= (unsigned int) n;

            if (n > 0 && reader->decoder != NULL) {
                lha_decoder_monitor(reader->decoder, fuzz_progress_callback, (void *) &progress_sink);
            }

            if ((Size + (size_t) i) % 5 == 0) {
                n = lha_reader_read(reader, buf, (Size % sizeof(buf)));
                progress_sink ^= (unsigned int) n;
            }
        }

        snprintf(outname, sizeof(outname), "./dummy_file_out_%d", i);
        (void) lha_reader_extract(reader, outname, fuzz_progress_callback, (void *) &progress_sink);

        if (reader->decoder != NULL) {
            lha_decoder_monitor(reader->decoder, fuzz_progress_callback, (void *) &progress_sink);
        }
    }

    lha_reader_free(reader);
    return 0;
}