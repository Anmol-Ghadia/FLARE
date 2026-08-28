// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetSizeProc at tif_open.c:712:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetSizeProc at tif_open.c:712:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00,
            0x00, 0x00
        };
        (void)fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static void set_basic_tags(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;

    if (Size > 0) width = (uint32_t)(Data[0] ? Data[0] : 1);
    if (Size > 1) height = (uint32_t)(Data[1] ? Data[1] : 1);
    if (Size > 2) spp = (uint16_t)((Data[2] % 4) + 1);
    if (Size > 3) {
        switch (Data[3] % 4) {
            case 0: bps = 1; break;
            case 1: bps = 8; break;
            case 2: bps = 16; break;
            default: bps = 32; break;
        }
    }
    if (Size > 4) {
        switch (Data[4] % 3) {
            case 0: photo = PHOTOMETRIC_MINISBLACK; break;
            case 1: photo = PHOTOMETRIC_RGB; break;
            default: photo = PHOTOMETRIC_MINISWHITE; break;
        }
    }
    if (photo == PHOTOMETRIC_RGB && spp < 3) spp = 3;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

    if (tiled) {
        uint32_t tw = width ? width : 1;
        uint32_t th = height ? height : 1;
        if (Size > 5) tw = (uint32_t)((Data[5] % 32) + 1);
        if (Size > 6) th = (uint32_t)((Data[6] % 32) + 1);
        (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    } else {
        uint32_t rowsperstrip = height ? height : 1;
        if (Size > 7) rowsperstrip = (uint32_t)((Data[7] % (height ? height : 1)) + 1);
        (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

static void exercise_writer(const uint8_t *Data, size_t Size, const char *mode, int tiled_first) {
    TIFF *tif = TIFFOpen("./dummy_file", mode);
    if (!tif) return;

    (void)TIFFGetSizeProc(tif);

    (void)TIFFCreateDirectory(tif);
    set_basic_tags(tif, Data, Size, tiled_first);
    (void)TIFFWriteCheck(tif, tiled_first ? 1 : 0, "fuzz_primary");
    (void)TIFFWriteDirectory(tif);
    TIFFFreeDirectory(tif);

    (void)TIFFCreateDirectory(tif);
    set_basic_tags(tif, Data, Size, tiled_first ? 0 : 1);
    (void)TIFFWriteCheck(tif, tiled_first ? 0 : 1, "fuzz_secondary");
    (void)TIFFWriteDirectory(tif);
    TIFFFreeDirectory(tif);

    TIFFCleanup(tif);
}

static void exercise_readonly_like_state(const uint8_t *Data, size_t Size) {
    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return;

    (void)TIFFGetSizeProc(tif);
    (void)TIFFCreateDirectory(tif);

    if (Size > 0 && (Data[0] & 1) == 0) {
        set_basic_tags(tif, Data, Size, 0);
        (void)TIFFWriteCheck(tif, 1, "mismatch_tile_check");
    } else {
        set_basic_tags(tif, Data, Size, 1);
        (void)TIFFWriteCheck(tif, 0, "mismatch_scanline_check");
    }

    TIFFFreeDirectory(tif);
    TIFFCleanup(tif);
}

int LLVMFuzzerTestOneInput_56(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    exercise_writer(Data, Size, "w", 0);
    exercise_writer(Data, Size, "w8", 1);
    exercise_readonly_like_state(Data, Size);

    return 0;
}