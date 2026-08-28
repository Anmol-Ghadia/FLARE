// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFNumberOfDirectories at tif_dir.c:1635:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFNumberOfDirectories at tif_dir.c:1635:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_17(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    (void)TIFFCurrentDirOffset(tif);
    (void)TIFFCurrentDirectory(tif);

    uint16_t ndirs = TIFFNumberOfDirectories(tif);
    (void)TIFFCurrentDirOffset(tif);
    (void)TIFFCurrentDirectory(tif);

    TIFFPrintDirectory(tif, stdout, 0L);
    TIFFPrintDirectory(tif, stdout, 1L);
    TIFFPrintDirectory(tif, stdout, (long)-1);

    if (ndirs > 0) {
        uint16_t idxs[6];
        idxs[0] = 0;
        idxs[1] = (uint16_t)(ndirs - 1);
        idxs[2] = (uint16_t)(ndirs / 2);
        idxs[3] = ndirs;
        idxs[4] = (uint16_t)(ndirs + 1);
        idxs[5] = 65535U;

        for (size_t i = 0; i < sizeof(idxs) / sizeof(idxs[0]); ++i) {
            if (TIFFSetDirectory(tif, idxs[i])) {
                (void)TIFFCurrentDirOffset(tif);
                (void)TIFFCurrentDirectory(tif);
                TIFFPrintDirectory(tif, stdout, (long)idxs[i]);
                (void)TIFFReadDirectory(tif);
                (void)TIFFCurrentDirOffset(tif);
                (void)TIFFCurrentDirectory(tif);
            }
        }
    } else {
        (void)TIFFSetDirectory(tif, 0);
        (void)TIFFReadDirectory(tif);
    }

    if (TIFFSetDirectory(tif, 0)) {
        for (int i = 0; i < 32; ++i) {
            (void)TIFFCurrentDirOffset(tif);
            (void)TIFFCurrentDirectory(tif);
            TIFFPrintDirectory(tif, stdout, (long)i);
            if (!TIFFReadDirectory(tif)) {
                break;
            }
        }
    }

    (void)TIFFNumberOfDirectories(tif);
    (void)TIFFCurrentDirOffset(tif);
    (void)TIFFCurrentDirectory(tif);

    TIFFClose(tif);
    return 0;
}