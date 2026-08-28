#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00,
            0x00, 0x00
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
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

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

    if (tiled) {
        uint32_t tw = width ? width : 1;
        uint32_t th = height ? height : 1;
        if (Size > 5) tw = (uint32_t)((Data[5] % 32) + 1);
        if (Size > 6) th = (uint32_t)((Data[6] % 32) + 1);
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    } else {
        uint32_t rowsperstrip = height ? height : 1;
        if (Size > 7) rowsperstrip = (uint32_t)((Data[7] % (height ? height : 1)) + 1);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w");
        if (tif) {
            (void)TIFFGetSizeProc(tif);

            (void)TIFFCreateDirectory(tif);
            set_basic_tags(tif, Data, Size, 0);
            (void)TIFFWriteCheck(tif, 0, "fuzz_scanline");
            (void)TIFFWriteDirectory(tif);

            TIFFFreeDirectory(tif);

            (void)TIFFCreateDirectory(tif);
            set_basic_tags(tif, Data, Size, 1);
            (void)TIFFWriteCheck(tif, 1, "fuzz_tile");
            (void)TIFFWriteDirectory(tif);

            TIFFFreeDirectory(tif);
            TIFFCleanup(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w8");
        if (tif) {
            (void)TIFFGetSizeProc(tif);

            (void)TIFFCreateDirectory(tif);
            set_basic_tags(tif, Data, Size, (Size > 0) ? (Data[0] & 1) : 0);
            (void)TIFFWriteCheck(tif, (Size > 1) ? (Data[1] & 1) : 0, "fuzz_bigtiff");
            (void)TIFFWriteDirectory(tif);

            TIFFFreeDirectory(tif);
            TIFFCleanup(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif) {
            (void)TIFFGetSizeProc(tif);
            (void)TIFFCreateDirectory(tif);
            (void)TIFFWriteCheck(tif, (Size > 0) ? (Data[0] & 1) : 0, "fuzz_readonly");
            TIFFFreeDirectory(tif);
            TIFFCleanup(tif);
        }
    }

    return 0;
}