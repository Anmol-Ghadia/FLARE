// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
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

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    {
        LHAInputStream *stream = lha_input_stream_from("./dummy_file");
        if (stream != NULL) {
            LHAReader *reader = lha_reader_new(stream);
            if (reader != NULL) {
                lha_reader_free(reader);
            } else {
                lha_input_stream_free(stream);
            }
        }
    }

    {
        FILE *rfp = fopen("./dummy_file", "rb");
        if (rfp != NULL) {
            LHAInputStream *stream = lha_input_stream_from_FILE(rfp);
            if (stream != NULL) {
                LHAReader *reader = lha_reader_new(stream);
                if (reader != NULL) {
                    lha_reader_free(reader);
                }
                lha_input_stream_free(stream);
            }
            fclose(rfp);
        }
    }

    {
        FILE *rfp = fopen("./dummy_file", "rb");
        if (rfp != NULL) {
            LHAInputStream *stream = lha_input_stream_from_FILE(rfp);
            if (stream != NULL) {
                lha_input_stream_free(stream);
            }
            fclose(rfp);
        }
    }

    return 0;
}