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
        (void)fwrite(Data, 1, Size, fp);
    }

    fclose(fp);
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
        reader1 = lha_reader_new(stream1);
        if (reader1 != NULL) {
            lha_reader_free(reader1);
            reader1 = NULL;
        }
        lha_input_stream_free(stream1);
        stream1 = NULL;
    }

    fp = fopen("./dummy_file", "rb");
    if (fp != NULL) {
        stream2 = lha_input_stream_from_FILE(fp);
        if (stream2 != NULL) {
            reader2 = lha_reader_new(stream2);
            if (reader2 != NULL) {
                lha_reader_free(reader2);
                reader2 = NULL;
            }
            lha_input_stream_free(stream2);
            stream2 = NULL;
        }
        fclose(fp);
        fp = NULL;
    }

    if (Size > 1) {
        LHAInputStream *stream3 = lha_input_stream_from("./dummy_file");
        if (stream3 != NULL) {
            lha_input_stream_free(stream3);
        }
    }

    remove("./dummy_file");
    return 0;
}