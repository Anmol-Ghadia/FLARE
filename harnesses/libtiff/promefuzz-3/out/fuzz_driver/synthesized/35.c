// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static size_t copy_string_from_data(char *dst, size_t dstsz,
                                    const uint8_t *Data, size_t Size,
                                    size_t offset) {
    size_t i = 0;
    if (dstsz == 0) return 0;
    while (offset + i < Size && i + 1 < dstsz) {
        unsigned char c = Data[offset + i];
        if (c == '\0') break;
        dst[i] = (char)c;
        i++;
    }
    dst[i] = '\0';
    return i;
}

int LLVMFuzzerTestOneInput_35(const uint8_t *Data, size_t Size) {
    static const char *modes[] = {
        "r", "r+", "w", "w8", "a", "a8",
        "rb", "wb", "r+b", "invalid", "", "rC", "rm", "w+m"
    };
    char altname1[64];
    char altname2[64];
    const char *oldname;
    const char *curname;
    thandle_t h;
    TIFF *tif = NULL;
    TIFF *tif_fd = NULL;
    int fd = -1;
    size_t mode_idx1 = 0, mode_idx2 = 0;

    write_dummy_file(Data, Size);

    copy_string_from_data(altname1, sizeof(altname1), Data, Size, 0);
    copy_string_from_data(altname2, sizeof(altname2), Data, Size, Size > 8 ? 8 : 0);

    if (Size > 0) mode_idx1 = Data[0] % (sizeof(modes) / sizeof(modes[0]));
    if (Size > 1) mode_idx2 = Data[1] % (sizeof(modes) / sizeof(modes[0]));

    tif = TIFFOpen("./dummy_file", modes[mode_idx1]);
    if (tif) {
        curname = TIFFFileName(tif);
        (void)curname;

        h = TIFFClientdata(tif);
        (void)h;

        oldname = TIFFSetFileName(tif, "./dummy_file");
        (void)oldname;
        curname = TIFFFileName(tif);
        (void)curname;

        if (altname1[0] != '\0') {
            oldname = TIFFSetFileName(tif, altname1);
            (void)oldname;
            curname = TIFFFileName(tif);
            (void)curname;
        }

        if (altname2[0] != '\0') {
            oldname = TIFFSetFileName(tif, altname2);
            (void)oldname;
            curname = TIFFFileName(tif);
            (void)curname;
        }

        oldname = TIFFSetFileName(tif, "");
        (void)oldname;
        curname = TIFFFileName(tif);
        (void)curname;

        TIFFCleanup(tif);
        tif = NULL;
    }

    fd = open("./dummy_file", O_RDONLY);
    if (fd >= 0) {
        tif_fd = TIFFFdOpen(fd, "./dummy_file", modes[mode_idx2]);
        if (tif_fd) {
            curname = TIFFFileName(tif_fd);
            (void)curname;

            h = TIFFClientdata(tif_fd);
            (void)h;

            oldname = TIFFSetFileName(tif_fd, "./dummy_file");
            (void)oldname;

            if (altname1[0] != '\0') {
                oldname = TIFFSetFileName(tif_fd, altname1);
                (void)oldname;
            }

            curname = TIFFFileName(tif_fd);
            (void)curname;

            TIFFCleanup(tif_fd);
            tif_fd = NULL;
            fd = -1; /* consumed by TIFFCleanup via closeproc */
        } else {
            close(fd);
            fd = -1;
        }
    }

    return 0;
}