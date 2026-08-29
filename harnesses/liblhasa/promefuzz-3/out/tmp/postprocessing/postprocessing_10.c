#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
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
    LHAReader *reader;
    volatile unsigned int progress_sink = 0;
    char outname[64];
    uint8_t buf[1024];
    size_t i, n;
    int fake_flag;
    int check_res;
    int extract_res;

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

    if (Size > 0) {
        snprintf(outname, sizeof(outname), "./dummy_out_%u", (unsigned int) Data[0]);
    } else {
        snprintf(outname, sizeof(outname), "./dummy_out");
    }

    fake_flag = lha_reader_current_is_fake(reader);
    progress_sink ^= (unsigned int) fake_flag;

    check_res = lha_reader_check(reader, progress_cb, (void *) &progress_sink);
    progress_sink ^= (unsigned int) check_res;

    extract_res = lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
    progress_sink ^= (unsigned int) extract_res;

    n = lha_reader_read(reader, buf, 0);
    progress_sink ^= (unsigned int) n;

    for (i = 0; i < 8; ++i) {
        size_t want = sizeof(buf);

        if (Size > i) {
            want = ((size_t) Data[i] % sizeof(buf)) + 1;
        }

        n = lha_reader_read(reader, buf, want);
        progress_sink ^= (unsigned int) n;

        fake_flag = lha_reader_current_is_fake(reader);
        progress_sink ^= (unsigned int) fake_flag;

        if ((i & 1) == 0) {
            check_res = lha_reader_check(reader, progress_cb, (void *) &progress_sink);
            progress_sink ^= (unsigned int) check_res;
        } else {
            extract_res = lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
            progress_sink ^= (unsigned int) extract_res;
        }

        if (n == 0) {
            break;
        }
    }

    check_res = lha_reader_check(reader, NULL, NULL);
    progress_sink ^= (unsigned int) check_res;

    extract_res = lha_reader_extract(reader, outname, NULL, NULL);
    progress_sink ^= (unsigned int) extract_res;

    fake_flag = lha_reader_current_is_fake(reader);
    progress_sink ^= (unsigned int) fake_flag;

    lha_reader_free(reader);
    return (int) progress_sink * 0;
}