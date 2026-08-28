// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFUnlinkDirectory at tif_dir.c:1734:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Data && Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void set_minimal_tags(TIFF *tif, const uint8_t *Data, size_t Size, uint32_t variant) {
    uint32_t width = 1 + (Size > 0 ? Data[0] : 0);
    uint32_t height = 1 + (Size > 1 ? Data[1] : 0);
    uint16_t spp = (variant & 1) ? 3 : 1;
    uint16_t bps = (variant & 2) ? 8 : 1;
    uint16_t photometric = (spp == 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = height ? height : 1;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
}

int LLVMFuzzerTestOneInput_66(const uint8_t *Data, size_t Size) {
    uint16_t dir_a = 0, dir_b = 0, dir_c = 0;
    TIFF *tif_rw = NULL;
    TIFF *tif_ro = NULL;

    write_dummy_file(Data, Size);

    tif_rw = TIFFOpen("./dummy_file", "w+");
    if (tif_rw) {
        (void)TIFFCreateDirectory(tif_rw);
        set_minimal_tags(tif_rw, Data, Size, 0);
        (void)TIFFWriteDirectory(tif_rw);

        (void)TIFFCreateDirectory(tif_rw);
        set_minimal_tags(tif_rw, Data, Size, 1);
        (void)TIFFWriteDirectory(tif_rw);

        (void)TIFFCreateDirectory(tif_rw);
        set_minimal_tags(tif_rw, Data, Size, 2);
        (void)TIFFWriteDirectory(tif_rw);

        dir_a = (Size > 0) ? (uint16_t)Data[0] : 0;
        dir_b = (Size > 1) ? (uint16_t)((Data[0] << 8) | Data[1]) : dir_a;
        dir_c = (Size > 2) ? (uint16_t)Data[2] : 2;

        (void)TIFFSetDirectory(tif_rw, 0);
        (void)TIFFSetDirectory(tif_rw, 1);
        (void)TIFFSetDirectory(tif_rw, 2);
        (void)TIFFSetDirectory(tif_rw, dir_a);
        (void)TIFFSetDirectory(tif_rw, dir_b);
        (void)TIFFSetDirectory(tif_rw, dir_c);

        (void)TIFFUnlinkDirectory(tif_rw, 0);
        (void)TIFFUnlinkDirectory(tif_rw, 1);
        (void)TIFFUnlinkDirectory(tif_rw, 2);
        (void)TIFFUnlinkDirectory(tif_rw, dir_a);
        (void)TIFFUnlinkDirectory(tif_rw, dir_b);

        TIFFFreeDirectory(tif_rw);
        (void)TIFFCreateDirectory(tif_rw);
        set_minimal_tags(tif_rw, Data, Size, 3);
        (void)TIFFWriteDirectory(tif_rw);

        TIFFClose(tif_rw);
    }

    tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro) {
        (void)TIFFSetDirectory(tif_ro, 0);
        (void)TIFFSetDirectory(tif_ro, 1);
        (void)TIFFSetDirectory(tif_ro, dir_a);
        (void)TIFFSetDirectory(tif_ro, dir_b);

        (void)TIFFCreateDirectory(tif_ro);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFUnlinkDirectory(tif_ro, 0);
        (void)TIFFUnlinkDirectory(tif_ro, dir_c);

        TIFFClose(tif_ro);
    }

    return 0;
}