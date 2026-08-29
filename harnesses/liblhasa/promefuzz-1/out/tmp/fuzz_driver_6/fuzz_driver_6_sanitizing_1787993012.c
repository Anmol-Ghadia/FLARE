#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lha_reader.h"

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

static void progress_cb(unsigned int blocks, unsigned int total, void *user_data) {
    volatile unsigned int *sink = (volatile unsigned int *) user_data;
    if (sink != NULL) {
        *sink ^= blocks;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *hdr;
    volatile unsigned int cb_sink = 0;
    uint8_t outbuf[4096];
    size_t offset = 0;
    int i;

    write_dummy_file(Data, Size);

    stream = lha_input_stream_from_file("./dummy_file");
    if (stream == NULL) {
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    if (Size > 0) {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[0] % 4));
        offset = 1;
    } else {
        lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) 0);
    }

    for (i = 0; i < 64; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        if (offset < Size) {
            lha_reader_set_dir_policy(reader, (LHAReaderDirPolicy) (Data[offset] % 4));
            offset++;
        }

        if (offset < Size) {
            switch (Data[offset] % 3) {
                case 0:
                    (void) lha_reader_check(reader, progress_cb, (void *) &cb_sink);
                    break;

                case 1: {
                    size_t chunk = 1 + ((offset + 1 < Size) ? Data[offset + 1] : 0);
                    chunk %= sizeof(outbuf);
                    if (chunk == 0) {
                        chunk = sizeof(outbuf);
                    }
                    (void) lha_reader_read(reader, outbuf, chunk);
                    (void) lha_reader_check(reader, progress_cb, (void *) &cb_sink);
                    break;
                }

                case 2: {
                    int reads = 1 + ((offset + 1 < Size) ? (Data[offset + 1] % 8) : 0);
                    int j;
                    for (j = 0; j < reads; ++j) {
                        size_t chunk = 1;
                        if (offset + 2 + (size_t) j < Size) {
                            chunk = 1 + (Data[offset + 2 + (size_t) j] % sizeof(outbuf));
                        }
                        if (lha_reader_read(reader, outbuf, chunk) == 0) {
                            break;
                        }
                    }
                    (void) lha_reader_check(reader, progress_cb, (void *) &cb_sink);
                    break;
                }
            }
            offset++;
        } else {
            (void) lha_reader_read(reader, outbuf, sizeof(outbuf));
            (void) lha_reader_check(reader, progress_cb, (void *) &cb_sink);
        }

        if (offset < Size && (Data[offset] & 1)) {
            size_t chunk = 1 + (Data[offset] % sizeof(outbuf));
            (void) lha_reader_read(reader, outbuf, chunk);
        }
        if (offset < Size) {
            offset++;
        }
    }

    lha_reader_free(reader);
    return 0;
}