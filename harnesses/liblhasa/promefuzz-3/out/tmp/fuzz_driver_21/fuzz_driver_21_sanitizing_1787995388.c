#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void progress_cb(unsigned int current,
                        unsigned int total,
                        void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    volatile unsigned int progress_sink = 0;
    uint8_t read_buf[4096];
    size_t i, chunk;
    LHAReader *reader;

    write_dummy_file(Data, Size);

    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        remove("./dummy_file");
        return 0;
    }

    (void) lha_reader_current_is_fake(reader);

    (void) lha_reader_check(reader, progress_cb, (void *)&progress_sink);
    (void) lha_reader_check(reader, NULL, NULL);

    (void) lha_reader_extract(reader, (char *)"./dummy_file.out",
                              progress_cb, (void *)&progress_sink);
    (void) lha_reader_extract(reader, (char *)"./dummy_file.out2",
                              NULL, NULL);

    for (i = 0; i < 8; ++i) {
        chunk = (Size > i) ? ((size_t) Data[i] % sizeof(read_buf)) : 0;
        if (chunk == 0) {
            chunk = sizeof(read_buf);
        }
        (void) lha_reader_read(reader, read_buf, chunk);
        (void) lha_reader_current_is_fake(reader);
    }

    (void) lha_reader_read(reader, read_buf, 0);
    (void) lha_reader_read(reader, read_buf, sizeof(read_buf));

    lha_reader_free(reader);

    remove("./dummy_file");
    remove("./dummy_file.out");
    remove("./dummy_file.out2");

    return 0;
}