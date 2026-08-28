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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1;
    uint32_t height = 1;
    uint16_t bits_per_sample = 8;
    uint16_t samples_per_pixel = 1;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planarconfig = PLANARCONFIG_CONTIG;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint32_t rows_per_strip = 1;

    if (Size >= 4) {
        width = ((uint32_t)Data[0] << 8) | Data[1];
        height = ((uint32_t)Data[2] << 8) | Data[3];
        if (width == 0) width = 1;
        if (height == 0) height = 1;
    }
    if (Size >= 5) {
        bits_per_sample = (Data[4] & 1) ? 16 : 8;
    }
    if (Size >= 6) {
        samples_per_pixel = (Data[5] % 4) + 1;
    }
    if (Size >= 7) {
        photometric = (samples_per_pixel >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        if ((Data[6] & 1) && samples_per_pixel == 1) {
            photometric = PHOTOMETRIC_MINISWHITE;
        }
    }
    if (Size >= 8) {
        rows_per_strip = (Data[7] % height) + 1;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bits_per_sample);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);

    if (Size & 1) {
        TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rows_per_strip);
    }

    tsize_t scanline_size = TIFFScanlineSize(tif);
    if (scanline_size <= 0) {
        scanline_size = (tsize_t)(width * samples_per_pixel * (bits_per_sample / 8));
        if (scanline_size <= 0) {
            scanline_size = 1;
        }
    }

    uint8_t localbuf[4096];
    void *scanbuf = localbuf;
    tsize_t bufsize = scanline_size;
    if ((size_t)bufsize > sizeof(localbuf)) {
        bufsize = sizeof(localbuf);
    }
    memset(localbuf, 0, sizeof(localbuf));
    if (Size > 0 && bufsize > 0) {
        size_t copy = (Size < (size_t)bufsize) ? Size : (size_t)bufsize;
        memcpy(localbuf, Data, copy);
    }

    uint32_t row = 0;
    uint16_t sample = 0;
    if (Size >= 10) {
        row = ((uint32_t)Data[8] << 8) | Data[9];
        if (height != 0) {
            row %= height;
        } else {
            row = 0;
        }
    }
    if (Size >= 11 && samples_per_pixel != 0) {
        sample = Data[10] % samples_per_pixel;
    }

    TIFFWriteScanline(tif, scanbuf, row, sample);
    TIFFWriteDirectory(tif);

    if (TIFFCreateEXIFDirectory(tif)) {
        if (Size >= 15) {
            uint16_t exif_version[4];
            exif_version[0] = Data[11];
            exif_version[1] = Data[12];
            exif_version[2] = Data[13];
            exif_version[3] = Data[14];
            TIFFSetField(tif, EXIFTAG_EXIFVERSION, exif_version);
        } else {
            uint16_t exif_version[4] = { '0', '2', '3', '1' };
            TIFFSetField(tif, EXIFTAG_EXIFVERSION, exif_version);
        }

        uint64_t dir_offset = 0;
        TIFFWriteCustomDirectory(tif, &dir_offset);
    }

    TIFFClose(tif);
    return 0;
}