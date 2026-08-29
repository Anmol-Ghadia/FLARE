// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_new at lha_input_stream.c:62:17 in lha_input_stream.h
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = NULL;
    LHAInputStream *stream1 = NULL;
    LHAInputStream *stream2 = NULL;
    LHAInputStream *stream3 = NULL;
    LHAReader *reader1 = NULL;
    LHAReader *reader2 = NULL;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
        fp = NULL;
    }

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

    fp = fopen("./dummy_file", "rb");
    if (fp != NULL) {
        stream3 = lha_input_stream_new(NULL, fp);
        if (stream3 != NULL) {
            /* Do not free: type is NULL and API contract forbids passing such
             * an object to lha_input_stream_free(). */
            stream3 = NULL;
        }
        fclose(fp);
        fp = NULL;
    }

    if (Size > 0) {
        char pathbuf[64];
        size_t n = Size < 32 ? Size : 32;
        size_t i;

        memcpy(pathbuf, "./", 2);
        for (i = 0; i < n; ++i) {
            uint8_t c = Data[i];
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                pathbuf[2 + i] = (char) c;
            } else {
                pathbuf[2 + i] = '_';
            }
        }
        pathbuf[2 + n] = '\0';

        stream1 = lha_input_stream_from(pathbuf);
        if (stream1 != NULL) {
            reader1 = lha_reader_new(stream1);
            if (reader1 != NULL) {
                lha_reader_free(reader1);
                reader1 = NULL;
            }
            lha_input_stream_free(stream1);
            stream1 = NULL;
        }
    }

    return 0;
}