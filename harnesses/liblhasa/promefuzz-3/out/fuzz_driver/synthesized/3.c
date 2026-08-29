// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_input_stream.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }
    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void progress_cb(unsigned int current, unsigned int total, void *user_data) {
    volatile unsigned int sink = current ^ total ^ (unsigned int) (uintptr_t) user_data;
    (void) sink;
}

int LLVMFuzzerTestOneInput_3(const uint8_t *Data, size_t Size) {
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *hdr;
    size_t i;
    unsigned int selector = 0;
    char outpath[64];

    write_dummy_file(Data, Size);

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
        selector = Data[0];
    }

    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (selector % 8));

    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, progress_cb, (void *) Data);
    (void) lha_reader_extract(reader, "./dummy_out_pre", progress_cb, (void *) Data);

    for (i = 0; i < 64; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        (void) lha_reader_current_is_fake(reader);

        if (((selector + (unsigned int) i) & 1U) == 0) {
            (void) lha_reader_check(reader, progress_cb, (void *) hdr);
        } else {
            (void) lha_reader_check(reader, NULL, NULL);
        }

        snprintf(outpath, sizeof(outpath), "./dummy_out_%zu_%u",
                 i, (unsigned int) hdr->header_level);

        if (((selector + (unsigned int) i) & 2U) == 0) {
            (void) lha_reader_extract(reader, outpath, progress_cb, (void *) hdr);
        } else {
            (void) lha_reader_extract(reader, outpath, NULL, NULL);
        }

        if ((i + 1) < Size) {
            lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[i + 1] % 8));
        }

        (void) lha_reader_current_is_fake(reader);
    }

    (void) lha_reader_next_file(reader);
    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, progress_cb, (void *) (uintptr_t) Size);
    (void) lha_reader_extract(reader, "./dummy_out_post", progress_cb,
                              (void *) (uintptr_t) Size);

    lha_reader_free(reader);
    return 0;
}