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

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#define OPEN _open
#define CLOSE _close
#else
#include <unistd.h>
#include <fcntl.h>
#define OPEN open
#define CLOSE close
#endif

static uint64_t read_u64(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    size_t lim = n < 8 ? n : 8;
    for (i = 0; i < lim; i++) {
        v |= ((uint64_t)p[i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    static const char *modes[] = {
        "r", "r+", "w", "w8", "a", "a8", "rC", "rm", "r+h", "w+"
    };
    char modebuf[16];
    const char *mode;
    FILE *fp;
    int fd = -1;
    TIFF *tif = NULL;
    size_t mode_index = 0;
    uint64_t off1 = 0, off2 = 0;
    int newfd1 = -1, newfd2 = -1;
    int oldfd;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size > 0) {
        mode_index = Data[0] % (sizeof(modes) / sizeof(modes[0]));
        mode = modes[mode_index];
    } else {
        mode = "r";
    }

#if defined(_WIN32)
    fd = OPEN("./dummy_file", _O_BINARY | _O_RDWR);
#else
    fd = OPEN("./dummy_file", O_RDWR);
#endif
    if (fd < 0) {
#if defined(_WIN32)
        fd = OPEN("./dummy_file", _O_BINARY | _O_RDONLY);
#else
        fd = OPEN("./dummy_file", O_RDONLY);
#endif
    }
    if (fd < 0) {
        return 0;
    }

    tif = TIFFFdOpen(fd, "./dummy_file", mode);
    if (tif == NULL) {
        CLOSE(fd);
        return 0;
    }

    (void)TIFFClientdata(tif);
    (void)TIFFCurrentDirOffset(tif);

    if (Size > 1) {
        off1 = read_u64(Data + 1, Size - 1);
        TIFFSetWriteOffset(tif, (toff_t)off1);
        (void)TIFFCurrentDirOffset(tif);
        (void)TIFFClientdata(tif);
    } else {
        TIFFSetWriteOffset(tif, (toff_t)0);
    }

    oldfd = TIFFSetFileno(tif, fd);
    (void)oldfd;
    (void)TIFFClientdata(tif);

    if (Size > 9) {
        off2 = read_u64(Data + 9, Size - 9);
        TIFFSetWriteOffset(tif, (toff_t)off2);
    } else {
        TIFFSetWriteOffset(tif, (toff_t)(Size * 17));
    }

    newfd1 = (Size > 17) ? (int)(Data[17]) : -1;
    (void)TIFFSetFileno(tif, newfd1);
    (void)TIFFClientdata(tif);
    (void)TIFFCurrentDirOffset(tif);

    newfd2 = (Size > 18) ? (int)(Data[18] | ((unsigned int)Data[0] << 8)) : fd;
    (void)TIFFSetFileno(tif, newfd2);
    (void)TIFFSetFileno(tif, fd);

    if (Size > 19) {
        size_t copylen = Size - 19;
        if (copylen > sizeof(modebuf) - 1) {
            copylen = sizeof(modebuf) - 1;
        }
        memcpy(modebuf, Data + 19, copylen);
        modebuf[copylen] = '\0';
        if (copylen > 0) {
            TIFF *tif2;
#if defined(_WIN32)
            int fd2 = OPEN("./dummy_file", _O_BINARY | _O_RDWR);
#else
            int fd2 = OPEN("./dummy_file", O_RDWR);
#endif
            if (fd2 >= 0) {
                tif2 = TIFFFdOpen(fd2, "./dummy_file", modebuf);
                if (tif2 != NULL) {
                    (void)TIFFClientdata(tif2);
                    TIFFSetWriteOffset(tif2, (toff_t)off1);
                    (void)TIFFCurrentDirOffset(tif2);
                    (void)TIFFSetFileno(tif2, fd2);
                    TIFFCleanup(tif2);
                    CLOSE(fd2);
                } else {
                    CLOSE(fd2);
                }
            }
        }
    }

    TIFFCleanup(tif);
    CLOSE(fd);
    return 0;
}