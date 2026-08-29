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
#include "lha_input_stream.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
    }

    (void) fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    LHAInputStream *stream1 = NULL;
    LHAInputStream *stream2 = NULL;
    LHAReader *reader1 = NULL;
    LHAReader *reader2 = NULL;
    FILE *fp = NULL;

    write_dummy_file(Data, Size);

    stream1 = lha_input_stream_from("./dummy_file");
    if (stream1 != NULL) {
        if (Size > 0 && (Data[0] & 1)) {
            reader1 = lha_reader_new(stream1);
            if (reader1 != NULL) {
                lha_reader_free(reader1);
                reader1 = NULL;
            }
            /* Ownership transfer is not explicitly documented; avoid double free. */
            stream1 = NULL;
        } else {
            lha_input_stream_free(stream1);
            stream1 = NULL;
        }
    }

    fp = fopen("./dummy_file", "rb");
    if (fp != NULL) {
        stream2 = lha_input_stream_from_FILE(fp);
        if (stream2 != NULL) {
            if (Size > 1 && (Data[0] & 2)) {
                reader2 = lha_reader_new(stream2);
                if (reader2 != NULL) {
                    lha_reader_free(reader2);
                    reader2 = NULL;
                }
                /* Avoid possible double free if reader assumes ownership. */
                stream2 = NULL;
            } else {
                lha_input_stream_free(stream2);
                stream2 = NULL;
            }
        }
        (void) fclose(fp);
        fp = NULL;
    }

    if (stream1 != NULL) {
        lha_input_stream_free(stream1);
        stream1 = NULL;
    }

    if (stream2 != NULL) {
        lha_input_stream_free(stream2);
        stream2 = NULL;
    }

    return 0;
}