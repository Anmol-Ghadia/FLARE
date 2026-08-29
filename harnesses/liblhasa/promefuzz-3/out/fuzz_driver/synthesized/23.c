// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_input_stream_from at lha_input_stream.c:381:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lha_reader.h"
#include "lha_decoder.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    volatile unsigned int *sink = (volatile unsigned int *) callback_data;
    if (sink != NULL) {
        *sink ^= current;
        *sink ^= total;
    }
}

int LLVMFuzzerTestOneInput_23(const uint8_t *Data, size_t Size)
{
    volatile unsigned int progress_sink = 0;
    FILE *fp;
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    uint8_t buf[4096];
    char outname[64];
    size_t n;
    int i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void) fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    stream = lha_input_stream_from("dummy_file");
    if (stream == NULL) {
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        return 0;
    }

    for (i = 0; i < 8; ++i) {
        LHAFileHeader *hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        (void)hdr;
        (void) lha_reader_current_is_fake(reader);

        if (((Size + (size_t)i) & 1U) != 0U) {
            (void) lha_reader_check(reader,
                                    fuzz_progress_callback,
                                    (void *)&progress_sink);
        }

        memset(buf, 0, sizeof(buf));
        n = lha_reader_read(reader, buf, sizeof(buf));
        progress_sink ^= (unsigned int)n;

        if (((Size + (size_t)i) % 3U) == 0U) {
            size_t small_len = Size % sizeof(buf);
            n = lha_reader_read(reader, buf, small_len);
            progress_sink ^= (unsigned int)n;
        }

        if (((Size + (size_t)i) % 2U) == 0U) {
            snprintf(outname, sizeof(outname), "./dummy_file_out_%d", i);
            (void) lha_reader_extract(reader,
                                      outname,
                                      fuzz_progress_callback,
                                      (void *)&progress_sink);
        } else {
            (void) lha_reader_check(reader,
                                    fuzz_progress_callback,
                                    (void *)&progress_sink);
        }
    }

    lha_reader_free(reader);
    return 0;
}