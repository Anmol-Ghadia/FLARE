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
            }
            lha_input_stream_free(stream);
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
        FILE *efp = fopen("./dummy_file", "ab");
        if (efp != NULL) {
            if (Size > 1) {
                fwrite(Data, 1, Size / 2, efp);
            }
            fclose(efp);
        }

        LHAInputStream *stream = lha_input_stream_from("./dummy_file");
        if (stream != NULL) {
            LHAReader *reader = lha_reader_new(stream);
            if (reader != NULL) {
                lha_reader_free(reader);
            }
            lha_input_stream_free(stream);
        }
    }

    return 0;
}