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
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    volatile unsigned int progress_sink = 0;
    uint8_t read_buf[4096];
    size_t i, chunk;
    LHAReader *reader = NULL;
    LHADecoder *decoder = NULL;

    write_dummy_file(Data, Size);

    /* Exercise lha_decoder_monitor directly with a synthetic decoder. */
    decoder = (LHADecoder *) calloc(1, sizeof(LHADecoder));
    if (decoder != NULL) {
        decoder->block_size = (Size >= 4)
                            ? ((unsigned int)Data[0] << 24)
                              | ((unsigned int)Data[1] << 16)
                              | ((unsigned int)Data[2] << 8)
                              | (unsigned int)Data[3]
                            : 0;
        decoder->stream_length = (uint64_t) Size;
        decoder->stream_pos = (Size >= 8)
                            ? ((uint64_t)Data[4] << 24)
                              | ((uint64_t)Data[5] << 16)
                              | ((uint64_t)Data[6] << 8)
                              | (uint64_t)Data[7]
                            : 0;
        decoder->last_block = (Size >= 9) ? Data[8] : 0;
        decoder->total_blocks = (Size >= 10) ? Data[9] : 0;

        lha_decoder_monitor(decoder, progress_cb, (void *)&progress_sink);

        decoder->stream_pos = decoder->stream_length;
        lha_decoder_monitor(decoder, progress_cb, (void *)&progress_sink);

        free(decoder);
        decoder = NULL;
    }

    /* Try to open the fuzz data as an archive if constructor is available. */
    reader = lha_reader_new("./dummy_file");
    if (reader == NULL) {
        remove("./dummy_file");
        return 0;
    }

    /* Query fake-state early, even before any successful extraction/read. */
    (void) lha_reader_current_is_fake(reader);

    /* Attempt integrity check with and without callback. */
    (void) lha_reader_check(reader, progress_cb, (void *)&progress_sink);
    (void) lha_reader_check(reader, NULL, NULL);

    /* Attempt extraction with different output paths/callback styles. */
    (void) lha_reader_extract(reader, (char *)"./dummy_file.out", progress_cb,
                              (void *)&progress_sink);
    (void) lha_reader_extract(reader, (char *)"./dummy_file.out2", NULL, NULL);

    /* Read decompressed data in variable chunk sizes. */
    for (i = 0; i < 8; ++i) {
        chunk = (Size > i) ? ((size_t)Data[i] % sizeof(read_buf)) : 0;
        if (chunk == 0) {
            chunk = sizeof(read_buf);
        }
        (void) lha_reader_read(reader, read_buf, chunk);
        (void) lha_reader_current_is_fake(reader);
    }

    /* Additional edge-case reads. */
    (void) lha_reader_read(reader, read_buf, 0);
    (void) lha_reader_read(reader, read_buf, sizeof(read_buf));

    lha_reader_free(reader);
    remove("./dummy_file");
    remove("./dummy_file.out");
    remove("./dummy_file.out2");

    return 0;
}