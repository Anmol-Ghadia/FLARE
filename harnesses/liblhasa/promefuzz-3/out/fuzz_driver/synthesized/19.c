// This fuzz driver is generated for library liblhasa, aiming to fuzz the following functions:
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_free at lha_reader.c:207:6 in lha_reader.h
// lha_reader_new at lha_reader.c:177:12 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
// lha_reader_check at lha_reader.c:406:5 in lha_reader.h
// lha_reader_extract at lha_reader.c:852:5 in lha_reader.h
// lha_reader_read at lha_reader.c:351:8 in lha_reader.h
// lha_reader_current_is_fake at lha_reader.c:881:5 in lha_reader.h
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

static void write_dummy_file(const uint8_t *data, size_t size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (size > 0) {
            fwrite(data, 1, size, fp);
        }
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size)
{
    volatile unsigned int progress_sink = 0;
    unsigned char read_buf[4096];
    size_t chunk;
    size_t off = 0;
    int is_fake;
    int rc_check;
    int rc_extract;
    LHAReader *reader = NULL;

    write_dummy_file(Data, Size);

    /* Try to open a reader using common liblhasa constructors if available. */
#if defined(__has_include)
#  if __has_include("lha_input_stream.h")
#    include "lha_input_stream.h"
#  endif
#endif

    /*
     * Prefer opening from file path. If the build environment provides
     * lha_reader_new(), this should compile against liblhasa public API.
     */
#if defined(LHA_READER_H) || defined(lha_reader_new) || 1
    reader = lha_reader_new("./dummy_file");
#endif

    if (reader == NULL) {
        remove("./dummy_file");
        return 0;
    }

    /* Exercise state-dependent APIs even if reader is not positioned. */
    is_fake = lha_reader_current_is_fake(reader);
    (void) is_fake;

    rc_check = lha_reader_check(reader, progress_cb, (void *) &progress_sink);
    (void) rc_check;

    rc_extract = lha_reader_extract(reader, "./dummy_file.out",
                                    progress_cb, (void *) &progress_sink);
    (void) rc_extract;

    chunk = (Size == 0) ? sizeof(read_buf) : ((size_t)Data[0] % sizeof(read_buf)) + 1;

    while (off < Size + 2 * sizeof(read_buf)) {
        size_t n = lha_reader_read(reader, read_buf, chunk);
        if (n == 0) {
            break;
        }
        off += n;

        /* Vary chunk sizes and re-check/extract during/after reads. */
        chunk = ((off < Size) ? (Data[off % Size] + 1) : (chunk + 17)) % sizeof(read_buf);
        if (chunk == 0) {
            chunk = 1;
        }

        if ((off & 1) == 0) {
            (void) lha_reader_current_is_fake(reader);
        }
        if ((off & 3) == 0) {
            (void) lha_reader_check(reader, progress_cb, (void *) &progress_sink);
        }
        if ((off & 7) == 0) {
            (void) lha_reader_extract(reader, "./dummy_file.out",
                                      progress_cb, (void *) &progress_sink);
        }
    }

    lha_reader_free(reader);
    remove("./dummy_file.out");
    remove("./dummy_file");
    return 0;
}