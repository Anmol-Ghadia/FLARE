// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lha_reader.h"

static void progress_cb(unsigned int blocks, unsigned int total, void *user_data)
{
    (void) blocks;
    (void) total;
    (void) user_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    LHAFileHeader *hdr;
    size_t i, chunk_count, offset;
    uint8_t readbuf[4096];
    char outname[64];

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    stream = lha_input_stream_from("./dummy_file");
    if (stream == NULL) {
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    if (Size > 0) {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[0] % 8));
    } else {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 0);
    }

    offset = (Size > 1) ? 1 : 0;
    chunk_count = (Size > offset) ? ((Data[offset] % 8) + 1) : 1;
    if (Size > offset) {
        offset++;
    }

    for (i = 0; i < 1024; i++) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        (void) lha_reader_current_is_fake(reader);

        {
            size_t j;
            for (j = 0; j < chunk_count; j++) {
                size_t want = sizeof(readbuf);
                size_t got;

                if (offset < Size) {
                    want = (size_t) Data[offset] % (sizeof(readbuf) + 1);
                    offset++;
                }

                if (want == 0) {
                    want = 1;
                }

                got = lha_reader_read(reader, readbuf, want);
                if (got == 0) {
                    break;
                }

                (void) lha_reader_current_is_fake(reader);
            }
        }

        snprintf(outname, sizeof(outname), "./dummy_file.out.%zu", i);
        (void) lha_reader_extract(reader, outname, progress_cb, NULL);
        (void) lha_reader_current_is_fake(reader);

        if (offset < Size) {
            lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[offset] % 8));
            offset++;
        }
    }

    lha_reader_free(reader);
    return 0;
}