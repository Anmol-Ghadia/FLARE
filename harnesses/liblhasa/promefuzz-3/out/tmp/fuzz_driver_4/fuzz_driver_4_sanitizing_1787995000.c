#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

static void exercise_from_filename(void) {
    LHAInputStream *stream = lha_input_stream_from("./dummy_file");
    if (stream != NULL) {
        LHAReader *reader = lha_reader_new(stream);
        if (reader != NULL) {
            lha_reader_free(reader);
        }
        lha_input_stream_free(stream);
    }
}

static void exercise_from_FILE_owned_by_caller(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "rb");
    if (fp == NULL) {
        return;
    }

    LHAInputStream *stream = lha_input_stream_from_FILE(fp);
    if (stream != NULL) {
        LHAReader *reader = lha_reader_new(stream);
        if (reader != NULL) {
            lha_reader_free(reader);
        }
        lha_input_stream_free(stream);
    }

    fclose(fp);

    (void)Data;
    (void)Size;
}

static void exercise_manual_stream_new(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "rb");
    if (fp == NULL) {
        return;
    }

    LHAInputStream *stream = lha_input_stream_new(NULL, fp);
    if (stream != NULL) {
        if (Size > 0 && (Data[0] & 1)) {
            LHAReader *reader = lha_reader_new(stream);
            if (reader != NULL) {
                lha_reader_free(reader);
            }
        }
        lha_input_stream_free(stream);
    }

    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    exercise_from_filename();

    if (Size == 0 || (Data[0] % 3) == 0) {
        exercise_from_FILE_owned_by_caller(Data, Size);
    }

    if (Size < 2 || (Data[0] % 3) == 1) {
        exercise_manual_stream_new(Data, Size);
    }

    if (Size >= 2 && (Data[1] % 2) == 0) {
        exercise_from_filename();
    }

    remove("./dummy_file");
    return 0;
}