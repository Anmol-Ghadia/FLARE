// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

static void exercise_from_FILE_owned_by_caller(void) {
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
}

static void exercise_manual_stream_new(void) {
    FILE *fp = fopen("./dummy_file", "rb");
    if (fp == NULL) {
        return;
    }

    /* Avoid constructing an invalid stream with a NULL type. */
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    exercise_from_filename();

    if (Size == 0 || (Data[0] % 3) == 0) {
        exercise_from_FILE_owned_by_caller();
    }

    if (Size < 2 || (Data[0] % 3) == 1) {
        exercise_manual_stream_new();
    }

    if (Size >= 2 && (Data[1] % 2) == 0) {
        exercise_from_filename();
    }

    remove("./dummy_file");
    return 0;
}