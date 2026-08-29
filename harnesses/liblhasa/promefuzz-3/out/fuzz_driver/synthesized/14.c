// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
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
    fclose(fp);
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    FILE *fp = NULL;

    write_dummy_file(Data, Size);

    switch (Size % 3) {
        case 0:
            stream = lha_input_stream_from("./dummy_file");
            if (stream != NULL) {
                reader = lha_reader_new(stream);
                if (reader != NULL) {
                    lha_reader_free(reader);
                    reader = NULL;
                }
                lha_input_stream_free(stream);
                stream = NULL;
            }
            break;

        case 1:
            fp = fopen("./dummy_file", "rb");
            if (fp != NULL) {
                stream = lha_input_stream_from_FILE(fp);
                if (stream != NULL) {
                    reader = lha_reader_new(stream);
                    if (reader != NULL) {
                        lha_reader_free(reader);
                        reader = NULL;
                    }
                    lha_input_stream_free(stream);
                    stream = NULL;
                }
                fclose(fp);
                fp = NULL;
            }
            break;

        case 2:
        default:
            stream = lha_input_stream_from("./dummy_file");
            if (stream != NULL) {
                lha_input_stream_free(stream);
                stream = NULL;
            }

            fp = fopen("./dummy_file", "rb");
            if (fp != NULL) {
                stream = lha_input_stream_from_FILE(fp);
                if (stream != NULL) {
                    reader = lha_reader_new(stream);
                    if (reader != NULL) {
                        lha_reader_free(reader);
                        reader = NULL;
                    }
                    lha_input_stream_free(stream);
                    stream = NULL;
                }
                fclose(fp);
                fp = NULL;
            }
            break;
    }

    return 0;
}