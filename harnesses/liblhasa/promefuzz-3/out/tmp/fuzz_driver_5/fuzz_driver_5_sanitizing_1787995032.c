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
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    /* Path 1: create stream from filename, then reader, then free both safely. */
    {
        LHAInputStream *stream = lha_input_stream_from("./dummy_file");
        if (stream != NULL) {
            LHAReader *reader = lha_reader_new(stream);
            if (reader != NULL) {
                lha_reader_free(reader);
            }
            lha_input_stream_free(stream);
        }
    }

    /* Path 2: wrap an existing FILE* without ownership transfer. */
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

    /* Path 3: exercise lha_input_stream_new with a valid type from another stream. */
    {
        FILE *rfp = fopen("./dummy_file", "rb");
        if (rfp != NULL) {
            LHAInputStream *borrowed = lha_input_stream_from_FILE(rfp);
            if (borrowed != NULL) {
                const LHAInputStreamType *type = borrowed->type;
                if (type != NULL) {
                    LHAInputStream *stream2 = lha_input_stream_new(type, rfp);
                    if (stream2 != NULL) {
                        LHAReader *reader2 = lha_reader_new(stream2);
                        if (reader2 != NULL) {
                            lha_reader_free(reader2);
                        }
                        lha_input_stream_free(stream2);
                    }
                }
                lha_input_stream_free(borrowed);
            }
            fclose(rfp);
        }
    }

    return 0;
}