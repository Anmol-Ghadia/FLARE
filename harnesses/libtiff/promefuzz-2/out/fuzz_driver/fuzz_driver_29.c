// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
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
#include <unistd.h>
#include <fcntl.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    int fd_rd = open("./dummy_file", O_RDONLY);
    if (fd_rd >= 0) {
        TIFF *tif = TIFFFdOpen(fd_rd, "./dummy_file", "r");
        if (tif) {
            (void)TIFFClientdata(tif);
            (void)TIFFFileName(tif);

            const char *alt1 = (Size > 0 && (Data[0] & 1)) ? "./dummy_file" : "fuzz_name_a";
            const char *oldname1 = TIFFSetFileName(tif, alt1);
            (void)oldname1;
            (void)TIFFFileName(tif);

            int newfd1 = (Size > 1) ? (int)(Data[1] % 8) - 1 : -1;
            (void)TIFFSetFileno(tif, newfd1);

            const char *alt2 = (Size > 2 && (Data[2] & 1)) ? "" : "fuzz_name_b";
            const char *oldname2 = TIFFSetFileName(tif, alt2);
            (void)oldname2;
            (void)TIFFFileName(tif);

            int newfd2 = (Size > 3) ? (int)Data[3] : fd_rd;
            (void)TIFFSetFileno(tif, newfd2);
            (void)TIFFClientdata(tif);

            (void)TIFFSetFileno(tif, fd_rd);
            TIFFCleanup(tif);
        } else {
            close(fd_rd);
        }
    }

    int fd_wr = open("./dummy_file", O_RDWR);
    if (fd_wr >= 0) {
        TIFF *tif2 = TIFFFdOpen(fd_wr, "./dummy_file", "r+");
        if (tif2) {
            (void)TIFFClientdata(tif2);
            (void)TIFFFileName(tif2);

            const char *name3 = (Size > 4 && (Data[4] & 1)) ? "x" : "./dummy_file";
            (void)TIFFSetFileName(tif2, name3);
            (void)TIFFFileName(tif2);

            int rebound_fd = (Size > 5) ? ((int)Data[5] - 128) : 0;
            (void)TIFFSetFileno(tif2, rebound_fd);
            (void)TIFFClientdata(tif2);

            (void)TIFFSetFileno(tif2, fd_wr);
            TIFFCleanup(tif2);
        } else {
            close(fd_wr);
        }
    }

    return 0;
}