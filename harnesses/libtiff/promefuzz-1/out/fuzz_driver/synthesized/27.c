// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFNumberOfDirectories at tif_dir.c:1635:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFNumberOfDirectories at tif_dir.c:1635:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFCurrentDirectory at tif_open.c:613:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void exercise_readonly(TIFF *tif, const uint8_t *Data, size_t Size) {
    if (!tif) return;

    (void)TIFFCurrentDirectory(tif);

    uint16_t ndirs = TIFFNumberOfDirectories(tif);
    (void)ndirs;

    if (Size > 0) {
        uint16_t dirn1 = (uint16_t)Data[0];
        (void)TIFFSetDirectory(tif, dirn1);
        (void)TIFFCurrentDirectory(tif);
    }

    if (Size > 1) {
        uint16_t dirn2 = (uint16_t)(((uint16_t)Data[0] << 8) | Data[1]);
        (void)TIFFSetDirectory(tif, dirn2);
        (void)TIFFCurrentDirectory(tif);
    }

    (void)TIFFSetDirectory(tif, 0);
    if (ndirs != 0 && ndirs != 65535) {
        (void)TIFFSetDirectory(tif, (uint16_t)(ndirs - 1));
    }

    (void)TIFFUnlinkDirectory(tif, 0);
    if (Size > 2) {
        (void)TIFFUnlinkDirectory(tif, (uint16_t)Data[2]);
    }

    TIFFFreeDirectory(tif);
    (void)TIFFCreateDirectory(tif);
    (void)TIFFCurrentDirectory(tif);
    TIFFFreeDirectory(tif);
}

static void exercise_writable(TIFF *tif, const uint8_t *Data, size_t Size) {
    if (!tif) return;

    (void)TIFFCurrentDirectory(tif);
    (void)TIFFNumberOfDirectories(tif);

    (void)TIFFCreateDirectory(tif);
    (void)TIFFCurrentDirectory(tif);

    (void)TIFFSetDirectory(tif, 0);
    if (Size > 0) {
        (void)TIFFSetDirectory(tif, (uint16_t)Data[0]);
    }
    if (Size > 1) {
        uint16_t dirn = (uint16_t)(((uint16_t)Data[0] << 8) | Data[1]);
        (void)TIFFSetDirectory(tif, dirn);
    }

    (void)TIFFUnlinkDirectory(tif, 0);
    if (Size > 2) {
        (void)TIFFUnlinkDirectory(tif, (uint16_t)Data[2]);
    }
    if (Size > 3) {
        uint16_t dirn = (uint16_t)(((uint16_t)Data[2] << 8) | Data[3]);
        (void)TIFFUnlinkDirectory(tif, dirn);
    }

    TIFFFreeDirectory(tif);
    (void)TIFFCreateDirectory(tif);
    TIFFFreeDirectory(tif);
}

int LLVMFuzzerTestOneInput_27(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        exercise_readonly(tif, Data, Size);
        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r+");
    if (tif) {
        exercise_writable(tif, Data, Size);
        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "a");
    if (tif) {
        exercise_writable(tif, Data, Size);
        TIFFClose(tif);
    }

    return 0;
}