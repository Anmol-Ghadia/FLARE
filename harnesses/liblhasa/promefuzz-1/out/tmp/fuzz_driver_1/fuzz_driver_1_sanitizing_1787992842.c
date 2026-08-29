#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_input_stream.h"

static void fuzz_progress(unsigned int current,
                          unsigned int total,
                          void *user_data)
{
    (void) current;
    (void) total;
    (void) user_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *hdr;
    size_t i;
    unsigned int selector = 0;
    char outname[64];

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    stream = lha_input_stream_from("dummy_file");
    if (stream == NULL) {
        remove("./dummy_file");
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        remove("./dummy_file");
        return 0;
    }

    if (Size > 0) {
        selector = Data[0];
    }

    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)(selector & 0x03));

    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, NULL, NULL);
    (void) lha_reader_extract(reader, NULL, NULL, NULL);

    for (i = 0; i < 32; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        (void) lha_reader_current_is_fake(reader);

        if (((selector + (unsigned int) i) & 1U) != 0) {
            (void) lha_reader_check(reader, fuzz_progress, NULL);
        } else {
            (void) lha_reader_check(reader, NULL, NULL);
        }

        if (((selector >> (i & 7U)) & 1U) != 0) {
            (void) lha_reader_extract(reader, NULL, fuzz_progress, NULL);
        } else {
            (void) snprintf(outname, sizeof(outname), "./dummy_out_%zu", i);
            (void) lha_reader_extract(
                reader,
                outname,
                (((selector >> ((i + 1U) & 7U)) & 1U) ? fuzz_progress : NULL),
                NULL
            );
        }

        (void) lha_reader_current_is_fake(reader);

        if (((selector >> ((i + 2U) & 7U)) & 1U) != 0) {
            (void) lha_reader_check(reader, fuzz_progress, NULL);
        }
    }

    lha_reader_free(reader);
    remove("./dummy_file");

    for (i = 0; i < 32; ++i) {
        (void) snprintf(outname, sizeof(outname), "./dummy_out_%zu", i);
        remove(outname);
    }

    return 0;
}