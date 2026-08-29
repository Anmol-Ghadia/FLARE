// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int sink = current ^ total;
    if (callback_data != NULL) {
        volatile uint8_t *p = (volatile uint8_t *) callback_data;
        *p ^= (uint8_t) sink;
    }
}

static void write_dummy_file(const uint8_t *data, size_t size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (size > 0) {
            (void) fwrite(data, 1, size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    LHAReader *reader;
    uint8_t callback_state = 0;
    uint8_t read_buf[256];
    size_t nread;
    size_t to_read1 = 0;
    size_t to_read2 = 0;
    char out_path[32];

    write_dummy_file(Data, Size);

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        remove("./dummy_file");
        return 0;
    }

    if (Size > 0) {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[0] % 4));
    } else {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 0);
    }

    if (Size > 1) {
        to_read1 = (size_t) Data[1];
        if (to_read1 > sizeof(read_buf)) {
            to_read1 = sizeof(read_buf);
        }
    }

    memset(read_buf, 0, sizeof(read_buf));
    nread = lha_reader_read(reader, read_buf, to_read1);
    (void) nread;

    (void) lha_reader_check(reader, fuzz_progress_callback, &callback_state);
    (void) lha_reader_check(reader, NULL, NULL);

    memcpy(out_path, "./dummy_file", sizeof("./dummy_file"));
    (void) lha_reader_extract(reader, out_path, fuzz_progress_callback, &callback_state);
    (void) lha_reader_extract(reader, out_path, NULL, NULL);

    if (Size > 2) {
        to_read2 = (size_t) Data[2];
        if (to_read2 > sizeof(read_buf)) {
            to_read2 = sizeof(read_buf);
        }
        nread = lha_reader_read(reader, read_buf, to_read2);
        (void) nread;
    }

    lha_reader_free(reader);
    remove("./dummy_file");
    return 0;
}