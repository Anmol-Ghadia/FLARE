#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_reader.h"
#include "lha_decoder.h"

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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    LHAReader *reader = NULL;
    volatile unsigned int progress_sink = 0;
    uint8_t small_buf[64];
    uint8_t *dyn_buf = NULL;
    size_t dyn_len;
    int is_fake;

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

    is_fake = lha_reader_current_is_fake(reader);
    progress_sink ^= (unsigned int) is_fake;

    (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
    (void) lha_reader_extract(reader, "./dummy_file.out", progress_cb,
                              (void *) &progress_sink);

    memset(small_buf, 0, sizeof(small_buf));
    (void) lha_reader_read(reader, small_buf, sizeof(small_buf));
    (void) lha_reader_read(reader, small_buf, 0);

    dyn_len = Size > 0 ? ((size_t) Data[0] + 1U) : 17U;
    dyn_buf = (uint8_t *) malloc(dyn_len);
    if (dyn_buf != NULL) {
        memset(dyn_buf, 0, dyn_len);
        (void) lha_reader_read(reader, dyn_buf, dyn_len);
        free(dyn_buf);
    }

    lha_reader_free(reader);
    remove("./dummy_file");
    remove("./dummy_file.out");
    return 0;
}