// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteScanline at tif_write.c:47:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defval) {
    if (off + 2 > n) return defval;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defval) {
    if (off + 4 > n) return defval;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = rd32(Data, Size, 0, 1) % 64 + 1;
    uint32_t height = rd32(Data, Size, 4, 0) % 64 + 1;
    uint16_t bps = (uint16_t)((rd16(Data, Size, 8, 8) % 8) + 1);
    uint16_t spp = (uint16_t)((rd16(Data, Size, 10, 1) % 4) + 1);
    uint16_t photo = (uint16_t)((rd16(Data, Size, 12, PHOTOMETRIC_MINISBLACK) % 2) ?
                                PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
    if (spp < 3 && photo == PHOTOMETRIC_RGB) {
        spp = 3;
    }
    uint16_t planar = (rd16(Data, Size, 14, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = rd32(Data, Size, 16, height) % (height + 1);
    if (rowsperstrip == 0) {
        rowsperstrip = 1;
    }

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline <= 0) {
        scanline = (tmsize_t)(((uint64_t)width * spp * bps + 7) / 8);
        if (scanline <= 0) {
            scanline = 1;
        }
    }

    uint8_t rowbuf[4096];
    tmsize_t to_copy = scanline;
    if (to_copy > (tmsize_t)sizeof(rowbuf)) {
        to_copy = (tmsize_t)sizeof(rowbuf);
    }
    if (Size > 0) {
        memcpy(rowbuf, Data, (size_t)((to_copy < (tmsize_t)Size) ? to_copy : (tmsize_t)Size));
        if ((size_t)to_copy > Size) {
            memset(rowbuf + Size, 0, (size_t)to_copy - Size);
        }
    } else {
        memset(rowbuf, 0, (size_t)to_copy);
    }

    uint32_t row = height ? (rd32(Data, Size, 20, 0) % (height + 1)) : 0;
    uint16_t sample = 0;
    if (planar == PLANARCONFIG_SEPARATE && spp > 0) {
        sample = (uint16_t)(rd16(Data, Size, 24, 0) % spp);
    }

    (void)TIFFWriteScanline(tif, rowbuf, row, sample);
    (void)TIFFWriteDirectory(tif);

    (void)TIFFCreateEXIFDirectory(tif);

    {
        const char *exif_ver = "0231";
        (void)TIFFSetField(tif, EXIFTAG_EXIFVERSION, exif_ver);
    }

    {
        uint64_t dir_offset = 0;
        (void)TIFFWriteCustomDirectory(tif, &dir_offset);
    }

    TIFFClose(tif);
    return 0;
}