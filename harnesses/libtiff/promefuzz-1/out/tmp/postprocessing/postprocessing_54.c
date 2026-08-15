#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I',42,0,8,0,0,0,0,0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w+");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
    }
    if (!tif) {
        return 0;
    }

    int original_fd = TIFFFileno(tif);
    int original_mode = TIFFSetMode(tif, O_RDONLY);
    (void)TIFFSetMode(tif, original_mode);

    (void)TIFFFileno(tif);
    (void)TIFFFlushData(tif);
    (void)TIFFDeferStrileArrayWriting(tif);

    if (Size > 0) {
        int modes[] = {
            O_RDONLY, O_RDWR, O_WRONLY, 0,
#ifdef O_APPEND
            O_APPEND,
#endif
#ifdef O_CREAT
            O_CREAT,
#endif
#ifdef O_TRUNC
            O_TRUNC,
#endif
        };
        size_t nmodes = sizeof(modes) / sizeof(modes[0]);
        int chosen_mode = modes[Data[0] % nmodes];
        int prev_mode = TIFFSetMode(tif, chosen_mode);
        (void)TIFFFlushData(tif);
        (void)TIFFDeferStrileArrayWriting(tif);
        (void)TIFFSetMode(tif, prev_mode);
    }

    if (Size > 1) {
        int fd_null = open("/dev/null", O_RDWR);
        if (fd_null >= 0) {
            int prev_fd = TIFFSetFileno(tif, fd_null);
            (void)TIFFFileno(tif);
            (void)TIFFFlushData(tif);
            (void)TIFFDeferStrileArrayWriting(tif);

            if (Size > 2) {
                int fd_dummy = open("./dummy_file", O_RDWR);
                if (fd_dummy >= 0) {
                    (void)TIFFSetFileno(tif, fd_dummy);
                    (void)TIFFFileno(tif);
                    (void)TIFFFlushData(tif);
                    (void)TIFFDeferStrileArrayWriting(tif);
                    close(fd_dummy);
                }
            }

            (void)TIFFSetFileno(tif, prev_fd);
            close(fd_null);
        }
    }

    if (Size > 3) {
        for (size_t i = 0; i < Size && i < 8; ++i) {
            int mode = (int)(Data[i]);
            int prev = TIFFSetMode(tif, mode);
            (void)TIFFFileno(tif);
            (void)TIFFFlushData(tif);
            (void)TIFFDeferStrileArrayWriting(tif);
            (void)TIFFSetMode(tif, prev);
        }
    }

    TIFFCleanup(tif);
    return 0;
}