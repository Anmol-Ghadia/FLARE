// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_set_dir_policy at lha_reader.c:227:6 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_input_stream.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void progress_cb(unsigned int blocks, unsigned int total, void *user_data) {
    volatile unsigned int *sink = (volatile unsigned int *)user_data;
    if (sink != NULL) {
        *sink ^= blocks;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput_6(const uint8_t *Data, size_t Size) {
    FILE *fp;
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *hdr;
    volatile unsigned int cb_sink = 0;
    uint8_t outbuf[4096];
    size_t offset = 0;
    int i;

    write_dummy_file(Data, Size);

    fp = fopen("./dummy_file", "rb");
    if (fp == NULL) {
        return 0;
    }

    stream = lha_input_stream_from_FILE(fp);
    if (stream == NULL) {
        fclose(fp);
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    if (Size > 0) {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)(Data[0] % 4));
        offset = 1;
    } else {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)0);
    }

    for (i = 0; i < 64; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        if (offset < Size) {
            lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy)(Data[offset] % 4));
            offset++;
        }

        if (offset < Size) {
            switch (Data[offset] % 3) {
                case 0:
                    (void)lha_reader_check(reader, progress_cb, (void *)&cb_sink);
                    break;

                case 1: {
                    size_t chunk = 1 + ((offset + 1 < Size) ? Data[offset + 1] : 0);
                    chunk %= sizeof(outbuf);
                    if (chunk == 0) {
                        chunk = sizeof(outbuf);
                    }
                    (void)lha_reader_read(reader, outbuf, chunk);
                    (void)lha_reader_check(reader, progress_cb, (void *)&cb_sink);
                    break;
                }

                case 2: {
                    int reads = 1 + ((offset + 1 < Size) ? (Data[offset + 1] % 8) : 0);
                    int j;
                    for (j = 0; j < reads; ++j) {
                        size_t chunk = 1;
                        if (offset + 2 + (size_t)j < Size) {
                            chunk = 1 + (Data[offset + 2 + (size_t)j] % sizeof(outbuf));
                        }
                        if (lha_reader_read(reader, outbuf, chunk) == 0) {
                            break;
                        }
                    }
                    (void)lha_reader_check(reader, progress_cb, (void *)&cb_sink);
                    break;
                }
            }
            offset++;
        } else {
            (void)lha_reader_read(reader, outbuf, sizeof(outbuf));
            (void)lha_reader_check(reader, progress_cb, (void *)&cb_sink);
        }

        if (offset < Size && (Data[offset] & 1)) {
            size_t chunk = 1 + (Data[offset] % sizeof(outbuf));
            (void)lha_reader_read(reader, outbuf, chunk);
        }
        if (offset < Size) {
            offset++;
        }
    }

    lha_reader_free(reader);
    return 0;
}