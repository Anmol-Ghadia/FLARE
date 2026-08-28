// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 2 > n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 4 > n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void setup_common_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0, 1) % 2048 + 1;
    uint32_t height = rd32(Data, Size, 4, 1) % 2048 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 0) % (sizeof(bps_choices)/sizeof(bps_choices[0]))];
    uint16_t photo_choices[] = {
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_MINISWHITE,
        PHOTOMETRIC_RGB,
        PHOTOMETRIC_PALETTE,
        PHOTOMETRIC_SEPARATED
    };
    uint16_t photometric = photo_choices[rd16(Data, Size, 12, 0) % (sizeof(photo_choices)/sizeof(photo_choices[0]))];
    uint16_t planar = (rd16(Data, Size, 14, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression_choices[] = {
        COMPRESSION_NONE,
        COMPRESSION_LZW,
        COMPRESSION_PACKBITS,
        COMPRESSION_DEFLATE
    };
    uint16_t compression = compression_choices[rd16(Data, Size, 16, 0) % (sizeof(compression_choices)/sizeof(compression_choices[0]))];
    uint16_t fillorder = (rd16(Data, Size, 18, 0) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    uint16_t orientation = ORIENTATION_TOPLEFT;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    if (photometric == PHOTOMETRIC_RGB && spp < 3) {
        photometric = PHOTOMETRIC_MINISBLACK;
    }
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);

    if (tiled) {
        uint32_t tilew = rd32(Data, Size, 20, 16) % 512 + 1;
        uint32_t tileh = rd32(Data, Size, 24, 16) % 512 + 1;
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    } else {
        uint32_t rowsperstrip = rd32(Data, Size, 20, 1) % 1024 + 1;
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w");
        if (tif) {
            setup_common_fields(tif, Data, Size, 0);

            tmsize_t scanline = TIFFScanlineSize(tif);
            uint32_t nrows = rd32(Data, Size, 28, 1);
            tmsize_t vstrip = TIFFVStripSize(tif, nrows);
            uint32_t strip = rd32(Data, Size, 32, 0);
            tmsize_t rawstrip = TIFFRawStripSize(tif, strip);

            size_t bufsize = 0;
            if (scanline > 0 && scanline < (1 << 20)) {
                bufsize = (size_t)scanline;
            } else if (vstrip > 0 && vstrip < (1 << 20)) {
                bufsize = (size_t)vstrip;
            } else if (Size > 0) {
                bufsize = Size < (1 << 20) ? Size : (1 << 20);
            }

            if (bufsize > 0) {
                void *buf = malloc(bufsize);
                if (buf) {
                    if (Size > 0) {
                        size_t copy = Size < bufsize ? Size : bufsize;
                        memcpy(buf, Data, copy);
                        if (copy < bufsize) memset((uint8_t *)buf + copy, 0, bufsize - copy);
                    } else {
                        memset(buf, 0, bufsize);
                    }

                    TIFFWriteEncodedStrip(tif, strip, buf, (tmsize_t)bufsize);
                    TIFFWriteEncodedStrip(tif, 0, buf, (tmsize_t)bufsize);

                    free(buf);
                }
            }

            (void)rawstrip;
            TIFFClose(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w");
        if (tif) {
            setup_common_fields(tif, Data, Size, 1);

            tmsize_t scanline = TIFFScanlineSize(tif);
            tmsize_t tilerow = TIFFTileRowSize(tif);
            uint32_t nrows = rd32(Data, Size, 36, 1);
            tmsize_t vtile = TIFFVTileSize(tif, nrows);
            uint32_t strip = rd32(Data, Size, 40, 0);
            tmsize_t rawstrip = TIFFRawStripSize(tif, strip);

            size_t bufsize = 0;
            if (vtile > 0 && vtile < (1 << 20)) {
                bufsize = (size_t)vtile;
            } else if (tilerow > 0 && tilerow < (1 << 20)) {
                bufsize = (size_t)tilerow;
            } else if (scanline > 0 && scanline < (1 << 20)) {
                bufsize = (size_t)scanline;
            } else if (Size > 0) {
                bufsize = Size < (1 << 20) ? Size : (1 << 20);
            }

            if (bufsize > 0) {
                void *buf = malloc(bufsize);
                if (buf) {
                    if (Size > 0) {
                        size_t copy = Size < bufsize ? Size : bufsize;
                        memcpy(buf, Data, copy);
                        if (copy < bufsize) memset((uint8_t *)buf + copy, 0xA5, bufsize - copy);
                    } else {
                        memset(buf, 0xA5, bufsize);
                    }

                    TIFFWriteEncodedStrip(tif, strip, buf, (tmsize_t)bufsize);
                    TIFFWriteEncodedStrip(tif, 0, buf, (tmsize_t)bufsize);

                    free(buf);
                }
            }

            (void)rawstrip;
            TIFFClose(tif);
        }
    }

    remove("./dummy_file");
    return 0;
}