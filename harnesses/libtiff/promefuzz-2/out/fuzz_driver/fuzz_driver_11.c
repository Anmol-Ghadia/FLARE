// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteScanline at tif_write.c:47:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
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
#include <stdlib.h>

#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t bits_per_sample = 8;
    uint16_t samples_per_pixel = 1;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;

    if (Size >= 4) {
        width = ((uint32_t)Data[0] | ((uint32_t)Data[1] << 8)) % 512 + 1;
        height = ((uint32_t)Data[2] | ((uint32_t)Data[3] << 8)) % 512 + 1;
    }
    if (Size >= 5) {
        bits_per_sample = (Data[4] & 1) ? 16 : 8;
    }
    if (Size >= 6) {
        samples_per_pixel = (Data[5] % 4) + 1;
    }
    if (Size >= 7) {
        photometric = (samples_per_pixel >= 3 && (Data[6] & 1)) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    }
    if (Size >= 8) {
        planar = (Data[7] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size >= 9) {
        compression = (Data[8] & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bits_per_sample);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, height ? height : 1);

    {
        uint32_t got_width = 0;
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &got_width);
    }

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline <= 0 || scanline > (1 << 20)) {
        TIFFClose(tif);
        return 0;
    }

    unsigned char *buf = (unsigned char *)_TIFFmalloc((tmsize_t)scanline);
    if (!buf) {
        TIFFClose(tif);
        return 0;
    }

    if (Size == 0) {
        memset(buf, 0, (size_t)scanline);
    }

    uint32_t max_rows = height + 2;
    if (max_rows > 8) max_rows = 8;

    for (uint32_t row = 0; row < max_rows; row++) {
        for (tmsize_t i = 0; i < scanline; i++) {
            buf[i] = (unsigned char)(Size ? Data[(row + (size_t)i) % Size] : (uint8_t)(row + i));
        }

        uint16_t sample = 0;
        if (planar == PLANARCONFIG_SEPARATE && samples_per_pixel > 0) {
            sample = (uint16_t)(row % samples_per_pixel);
        }

        (void)TIFFWriteScanline(tif, buf, row, sample);
    }

    _TIFFfree(buf);
    TIFFClose(tif);
    return 0;
}