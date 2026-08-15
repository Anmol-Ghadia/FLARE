#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#ifndef OPEN_FN
#define OPEN_FN _open
#endif
#ifndef CLOSE_FN
#define CLOSE_FN _close
#endif
#ifndef DUP_FN
#define DUP_FN _dup
#endif
#else
#include <unistd.h>
#ifndef OPEN_FN
#define OPEN_FN open
#endif
#ifndef CLOSE_FN
#define CLOSE_FN close
#endif
#ifndef DUP_FN
#define DUP_FN dup
#endif
#endif

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void exercise_tiff(TIFF *tif, const uint8_t *Data, size_t Size) {
    if (!tif) return;

    (void)TIFFGetMode(tif);
    (void)TIFFIsBigEndian(tif);
    (void)TIFFIsUpSampled(tif);
    (void)TIFFFlushData(tif);

    if (Size > 0) {
        uint16_t dirn1 = (uint16_t)Data[0];
        (void)TIFFUnlinkDirectory(tif, dirn1);
    } else {
        (void)TIFFUnlinkDirectory(tif, 0);
    }

    if (Size > 1) {
        uint16_t dirn2 = (uint16_t)(((uint16_t)Data[0] << 8) | Data[1]);
        (void)TIFFUnlinkDirectory(tif, dirn2);
    }

    if (Size > 2) {
        uint16_t dirn3 = (uint16_t)(Size & 0xFFFFu);
        (void)TIFFUnlinkDirectory(tif, dirn3);
    }

    (void)TIFFFlushData(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    {
        int fd = OPEN_FN("./dummy_file", O_RDONLY);
        if (fd >= 0) {
            TIFF *tif = TIFFFdOpen(fd, "./dummy_file", "r");
            if (tif) {
                exercise_tiff(tif, Data, Size);
                TIFFClose(tif);
            } else {
                CLOSE_FN(fd);
            }
        }
    }

    {
        int fd = OPEN_FN("./dummy_file", O_RDWR);
        if (fd >= 0) {
            TIFF *tif = TIFFFdOpen(fd, "./dummy_file", "r+");
            if (tif) {
                exercise_tiff(tif, Data, Size);
                TIFFClose(tif);
            } else {
                CLOSE_FN(fd);
            }
        }
    }

    {
        int fd = OPEN_FN("./dummy_file", O_RDWR);
        if (fd >= 0) {
            int dupfd = DUP_FN(fd);
            TIFF *tif = TIFFFdOpen(fd, "./dummy_file", "a");
            if (tif) {
                exercise_tiff(tif, Data, Size);
                TIFFClose(tif);
            } else {
                CLOSE_FN(fd);
            }
            if (dupfd >= 0) {
                TIFF *tif2 = TIFFFdOpen(dupfd, "./dummy_file", "r+");
                if (tif2) {
                    exercise_tiff(tif2, Data, Size);
                    TIFFClose(tif2);
                } else {
                    CLOSE_FN(dupfd);
                }
            }
        }
    }

    return 0;
}