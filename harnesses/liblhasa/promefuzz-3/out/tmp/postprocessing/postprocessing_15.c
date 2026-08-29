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
    LHAReader *reader1 = NULL;
    LHAReader *reader2 = NULL;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
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

    if (Size > 1) {
        size_t split = Data[0] % (Size + 1);

        fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (split > 0 && split <= Size) {
                (void)fwrite(Data, 1, split, fp);
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
    }

    return 0;
}