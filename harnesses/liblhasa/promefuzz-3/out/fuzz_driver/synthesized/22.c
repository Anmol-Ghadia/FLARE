// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int sink = current ^ total;
    if (callback_data != NULL) {
        sink ^= *(const uint8_t *) callback_data;
    }
    (void) sink;
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }

    fclose(fp);
}

int LLVMFuzzerTestOneInput_22(const uint8_t *Data, size_t Size)
{
    LHAReader *reader;
    uint8_t cb_data = 0;
    unsigned char buf[4096];
    char outname[64];
    size_t read_len;

    write_dummy_file(Data, Size);

    if (Size > 0) {
        cb_data = Data[0];
    }

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        return 0;
    }

    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 0);
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 1);
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 2);
    lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 255);

    read_len = sizeof(buf);
    if (Size > 1) {
        read_len = (size_t) Data[1] % sizeof(buf);
    }

    (void) lha_reader_read(reader, buf, 0);
    (void) lha_reader_read(reader, buf, read_len);
    (void) lha_reader_read(reader, buf, sizeof(buf));

    (void) lha_reader_check(reader, fuzz_progress_callback, &cb_data);
    (void) lha_reader_check(reader, NULL, NULL);

    (void) snprintf(outname, sizeof(outname), "./dummy_file.out");
    (void) lha_reader_extract(reader, outname, fuzz_progress_callback, &cb_data);
    (void) lha_reader_extract(reader, outname, NULL, NULL);

    lha_reader_free(reader);
    return 0;
}