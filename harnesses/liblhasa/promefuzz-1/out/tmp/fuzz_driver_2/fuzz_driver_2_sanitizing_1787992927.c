#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lha_reader.h"
#include "lha_input_stream.h"

static void progress_cb(unsigned int current, unsigned int total, void *user_data)
{
    (void)current;
    (void)total;
    (void)user_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = NULL;
    FILE *rfp = NULL;
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    LHAFileHeader *hdr;
    uint8_t selector;
    char outname[64];
    unsigned int idx = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
    fp = NULL;

    rfp = fopen("./dummy_file", "rb");
    if (rfp == NULL) {
        remove("./dummy_file");
        return 0;
    }

    stream = lha_input_stream_from_FILE(rfp);
    if (stream == NULL) {
        fclose(rfp);
        remove("./dummy_file");
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        fclose(rfp);
        remove("./dummy_file");
        return 0;
    }

    selector = (Size > 0) ? Data[0] : 0;
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)(selector & 0x03));
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)((selector >> 2) & 0x03));

    while ((hdr = lha_reader_next_file(reader)) != NULL) {
        int is_fake = lha_reader_current_is_fake(reader);

        if ((selector & 0x10) != 0) {
            (void)lha_reader_check(reader, progress_cb, NULL);
        } else {
            (void)lha_reader_check(reader, NULL, NULL);
        }

        if ((selector & 0x20) != 0) {
            if (is_fake || (selector & 0x40) != 0) {
                (void)lha_reader_extract(reader, NULL, progress_cb, NULL);
            } else {
                snprintf(outname, sizeof(outname), "./dummy_out_%u", idx++);
                (void)lha_reader_extract(reader, outname, progress_cb, NULL);
                remove(outname);
            }
        } else {
            if (is_fake || (selector & 0x80) != 0) {
                (void)lha_reader_extract(reader, NULL, NULL, NULL);
            } else {
                snprintf(outname, sizeof(outname), "./dummy_out_%u", idx++);
                (void)lha_reader_extract(reader, outname, NULL, NULL);
                remove(outname);
            }
        }

        (void)hdr;
        selector = (uint8_t)((selector << 1) | (selector >> 7));
    }

    (void)lha_reader_current_is_fake(reader);
    (void)lha_reader_check(reader, NULL, NULL);
    (void)lha_reader_extract(reader, NULL, NULL, NULL);

    lha_reader_free(reader);
    reader = NULL;

    fclose(rfp);
    rfp = NULL;

    remove("./dummy_file");
    return 0;
}