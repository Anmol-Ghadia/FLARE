// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_input_stream_from_FILE at lha_input_stream.c:401:17 in lha_input_stream.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_input_stream_free at lha_input_stream.c:81:6 in lha_input_stream.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_next_file at lha_reader.c:292:16 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lha_reader.h"
#include "lha_input_stream.h"

static void fuzz_progress_callback(unsigned int current,
                                   unsigned int total,
                                   void *callback_data)
{
    (void) current;
    (void) total;
    (void) callback_data;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = NULL;
    FILE *in = NULL;
    LHAInputStream *stream = NULL;
    LHAReader *reader = NULL;
    LHAFileHeader *hdr;
    size_t i;
    int mode = 0;

    fp = fopen("./dummy_file", "wb");
    if (fp == NULL) {
        return 0;
    }

    if (Size > 0) {
        (void) fwrite(Data, 1, Size, fp);
        mode = Data[0] % 4;
    }

    fclose(fp);
    fp = NULL;

    in = fopen("./dummy_file", "rb");
    if (in == NULL) {
        remove("./dummy_file");
        return 0;
    }

    stream = lha_input_stream_from_FILE(in);
    if (stream == NULL) {
        fclose(in);
        remove("./dummy_file");
        return 0;
    }

    reader = lha_reader_new(stream);
    if (reader == NULL) {
        lha_input_stream_free(stream);
        fclose(in);
        remove("./dummy_file");
        return 0;
    }

    (void) lha_reader_current_is_fake(reader);
    (void) lha_reader_check(reader, fuzz_progress_callback, NULL);
    (void) lha_reader_extract(reader, NULL, fuzz_progress_callback, NULL);

    for (i = 0; i < 64; ++i) {
        hdr = lha_reader_next_file(reader);
        if (hdr == NULL) {
            break;
        }

        (void) lha_reader_current_is_fake(reader);

        switch ((mode + (int) i) % 4) {
            case 0:
                (void) lha_reader_check(reader, NULL, NULL);
                (void) lha_reader_extract(reader, NULL,
                                          fuzz_progress_callback, NULL);
                break;

            case 1:
                (void) lha_reader_extract(reader, "./dummy_file",
                                          fuzz_progress_callback, NULL);
                (void) lha_reader_check(reader, fuzz_progress_callback, NULL);
                break;

            case 2:
                (void) lha_reader_check(reader, fuzz_progress_callback, NULL);
                (void) lha_reader_current_is_fake(reader);
                (void) lha_reader_extract(reader, "./dummy_file", NULL, NULL);
                break;

            default:
                (void) lha_reader_extract(reader, NULL, NULL, NULL);
                (void) lha_reader_check(reader, NULL, NULL);
                break;
        }

        (void) lha_reader_current_is_fake(reader);
    }

    lha_reader_free(reader);
    lha_input_stream_free(stream);
    fclose(in);
    remove("./dummy_file");
    return 0;
}