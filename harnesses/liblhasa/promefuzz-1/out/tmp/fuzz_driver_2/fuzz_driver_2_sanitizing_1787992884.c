#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lha_reader.h"

static void progress_cb(unsigned int current, unsigned int total, void *user_data)
{
    (void) current;
    (void) total;
    (void) user_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    LHAReader *reader;
    LHAFileHeader *hdr;
    size_t i;
    unsigned int policy_seed = 0;
    char outname[64];

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

    for (i = 0; i < Size && i < sizeof(policy_seed); ++i) {
        policy_seed = (policy_seed << 8) | Data[i];
    }

    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)(policy_seed & 0xFF));
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)((policy_seed >> 2) & 0xFF));

    while ((hdr = lha_reader_next_file(reader)) != NULL) {
        int is_fake;
        int use_null_name;
        int do_check_first;
        int do_extract;
        int do_check_after;

        is_fake = lha_reader_current_is_fake(reader);

        use_null_name = (Size > 0) ? (Data[0] & 1) : 0;
        do_check_first = (Size > 1) ? (Data[1] & 1) : 1;
        do_extract = (Size > 2) ? (Data[2] & 1) : 1;
        do_check_after = (Size > 3) ? (Data[3] & 1) : 1;

        if (do_check_first) {
            (void) lha_reader_check(reader, progress_cb, NULL);
            (void) lha_reader_check(reader, NULL, NULL);
        }

        if (do_extract) {
            if (use_null_name || is_fake) {
                (void) lha_reader_extract(reader, NULL, progress_cb, NULL);
                (void) lha_reader_extract(reader, NULL, NULL, NULL);
            } else {
                snprintf(outname, sizeof(outname), "./dummy_out_%u", (unsigned int)(i & 0xffff));
                (void) lha_reader_extract(reader, outname, progress_cb, NULL);
                (void) lha_reader_extract(reader, outname, NULL, NULL);
                remove(outname);
            }
        }

        if (do_check_after) {
            (void) lha_reader_check(reader, progress_cb, NULL);
            (void) lha_reader_current_is_fake(reader);
        }
    }

    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, progress_cb, NULL);
    (void) lha_reader_extract(reader, NULL, progress_cb, NULL);

    lha_reader_free(reader);
    remove("./dummy_file");
    return 0;
}