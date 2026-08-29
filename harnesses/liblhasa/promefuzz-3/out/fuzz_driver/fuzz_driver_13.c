// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
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
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *hdr;
    volatile unsigned int progress_sink = 0;
    char outname[64];
    uint8_t buf[4096];
    size_t i, max_files;

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
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    max_files = (Size > 0) ? ((size_t) Data[0] % 16u + 1u) : 4u;

    for (i = 0; i < max_files; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        if ((i & 1u) == 0u) {
            (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
        } else {
            (void) lha_reader_check(reader, NULL, NULL);
        }

        if ((i & 2u) == 0u) {
            size_t read_sz = sizeof(buf);
            if (Size > (i + 1u)) {
                read_sz = (size_t) (Data[i + 1u] % (sizeof(buf) + 1u));
            }
            (void) lha_reader_read(reader, buf, read_sz);
            (void) lha_reader_read(reader, buf, sizeof(buf));
        } else {
            (void) lha_reader_read(reader, buf, 1);
            (void) lha_reader_read(reader, buf, 0);
        }

        snprintf(outname, sizeof(outname), "./dummy_file_out_%zu", i);
        if ((i & 4u) == 0u) {
            (void) lha_reader_extract(reader, outname, progress_cb, (void *) &progress_sink);
        } else {
            (void) lha_reader_extract(reader, outname, NULL, NULL);
        }
    }

    lha_reader_free(reader);
    return 0;
}