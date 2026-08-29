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

    if (Size > 0 && Data != NULL) {
        fwrite(Data, 1, Size, fp);
    }

    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    LHAInputStream *stream1 = NULL;
    LHAInputStream *stream2 = NULL;
    LHAInputStream *stream3 = NULL;
    LHAReader *reader1 = NULL;
    LHAReader *reader2 = NULL;
    FILE *fp = NULL;

    write_dummy_file(Data, Size);

    /* Path-based constructor */
    stream1 = lha_input_stream_from("./dummy_file");
    if (stream1 != NULL) {
        reader1 = lha_reader_new(stream1);
        if (reader1 != NULL) {
            lha_reader_free(reader1);
            reader1 = NULL;
            stream1 = NULL; /* ownership transferred/freed by reader */
        } else {
            lha_input_stream_free(stream1);
            stream1 = NULL;
        }
    }

    /* FILE*-based constructor */
    fp = fopen("./dummy_file", "rb");
    if (fp != NULL) {
        stream2 = lha_input_stream_from_FILE(fp);
        if (stream2 != NULL) {
            reader2 = lha_reader_new(stream2);
            if (reader2 != NULL) {
                lha_reader_free(reader2);
                reader2 = NULL;
                stream2 = NULL; /* wrapper freed by reader; FILE* still ours */
            } else {
                lha_input_stream_free(stream2);
                stream2 = NULL;
            }
        }
        fclose(fp);
        fp = NULL;
    }

    /* Direct wrapper creation with a valid NULL-close configuration:
       type is NULL here only to exercise allocation path; do not free it
       via lha_input_stream_free because API contract forbids it. */
    if (Size > 0 && (Data[0] & 1)) {
        stream3 = lha_input_stream_new(NULL, NULL);
        if (stream3 != NULL) {
            /* Intentionally leak-avoid by plain free is not safe for opaque API;
               leave it allocated only if constructor succeeded in this edge path. */
        }
    }

    return 0;
}