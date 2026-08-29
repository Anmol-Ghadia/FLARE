#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

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

static void exercise_stream(LHAInputStream *stream) {
    LHAReader *reader;

    if (stream == NULL) {
        return;
    }

    reader = lha_reader_new(stream);
    if (reader != NULL) {
        lha_reader_free(reader);
    } else {
        lha_input_stream_free(stream);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    LHAInputStream *stream;
    size_t split;

    write_dummy_file(Data, Size);

    stream = lha_input_stream_from("./dummy_file");
    exercise_stream(stream);

    fp = fopen("./dummy_file", "rb");
    if (fp != NULL) {
        stream = lha_input_stream_from_FILE(fp);
        if (stream != NULL) {
            exercise_stream(stream);
        }
        fclose(fp);
    }

    if (Size > 0) {
        split = Data[0] % (Size + 1);

        fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (split > 0) {
                (void)fwrite(Data, 1, split, fp);
            }
            fclose(fp);

            stream = lha_input_stream_from("./dummy_file");
            exercise_stream(stream);
        }
    }

    return 0;
}