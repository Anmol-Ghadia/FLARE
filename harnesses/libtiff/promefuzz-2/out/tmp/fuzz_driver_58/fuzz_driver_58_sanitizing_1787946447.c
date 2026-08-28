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

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        (void)TIFFIsTiled(tif);
        (void)TIFFIsUpSampled(tif);

        uint32_t u32 = 0;
        uint16_t u16 = 0;
        float f32 = 0.0f;
        char *str = NULL;

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &u32);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &u32);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &u32);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &u32);
        (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &u16);
        (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &u16);
        (void)TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &u16);
        (void)TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &u16);
        (void)TIFFGetField(tif, TIFFTAG_ORIENTATION, &u16);
        (void)TIFFGetField(tif, TIFFTAG_XRESOLUTION, &f32);
        (void)TIFFGetField(tif, TIFFTAG_YRESOLUTION, &f32);
        (void)TIFFGetField(tif, TIFFTAG_SOFTWARE, &str);
        (void)TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &str);

        if (TIFFIsTiled(tif)) {
            uint32_t tile_w = 0, tile_h = 0;
            (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tile_w);
            (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tile_h);

            if (tile_w == 0) tile_w = 16;
            if (tile_h == 0) tile_h = 16;

            size_t pixels = (size_t)tile_w * (size_t)tile_h;
            if (pixels > 0 && pixels < (1U << 24)) {
                uint32_t *raster = (uint32_t *)malloc(pixels * sizeof(uint32_t));
                if (raster) {
                    memset(raster, 0, pixels * sizeof(uint32_t));
                    (void)TIFFReadRGBATile(tif, 0, 0, raster);
                    if (Size >= 8) {
                        uint32_t col = rd32(Data, Size, 0, 0);
                        uint32_t row = rd32(Data, Size, 4, 0);
                        (void)TIFFReadRGBATile(tif, col, row, raster);
                    }
                    free(raster);
                }
            }
        }

        TIFFClose(tif);
    }

    TIFF *wtif = TIFFOpen("./dummy_file", "w");
    if (wtif) {
        uint32_t width = rd32(Data, Size, 0, 1) % 512 + 1;
        uint32_t height = rd32(Data, Size, 4, 1) % 512 + 1;
        uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
        uint16_t bps_choices[] = {8, 16, 32};
        uint16_t bps = bps_choices[rd16(Data, Size, 10, 0) % 3];
        uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        int make_tiled = (Size > 14) ? (Data[14] & 1) : 0;

        TIFFSetField(wtif, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(wtif, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(wtif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(wtif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(wtif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(wtif, TIFFTAG_PLANARCONFIG, planar);
        TIFFSetField(wtif, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(wtif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

        if (make_tiled) {
            uint32_t tw = rd32(Data, Size, 16, 16) % 128 + 16;
            uint32_t th = rd32(Data, Size, 20, 16) % 128 + 16;
            TIFFSetField(wtif, TIFFTAG_TILEWIDTH, tw);
            TIFFSetField(wtif, TIFFTAG_TILELENGTH, th);
        } else {
            uint32_t rowsperstrip = rd32(Data, Size, 16, 1) % 128 + 1;
            TIFFSetField(wtif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        }

        (void)TIFFIsTiled(wtif);
        (void)TIFFIsUpSampled(wtif);
        (void)TIFFWriteCheck(wtif, make_tiled, "fuzz");

        tsize_t scanline = TIFFScanlineSize(wtif);
        if (scanline > 0 && scanline < (1 << 20)) {
            unsigned char *buf = (unsigned char *)malloc((size_t)scanline);
            if (buf) {
                if (Size > 0) {
                    for (tsize_t i = 0; i < scanline; ++i) {
                        buf[i] = Data[(size_t)i % Size];
                    }
                } else {
                    memset(buf, 0, (size_t)scanline);
                }

                uint32_t row1 = 0;
                uint16_t sample1 = 0;
                (void)TIFFWriteScanline(wtif, buf, row1, sample1);

                uint32_t row2 = (Size >= 28) ? (rd32(Data, Size, 24, 0) % (height + 8)) : (height > 1 ? 1 : 0);
                uint16_t sample2 = (Size >= 30) ? (uint16_t)(rd16(Data, Size, 28, 0) % (spp + 2)) : 0;
                (void)TIFFWriteScanline(wtif, buf, row2, sample2);

                free(buf);
            }
        }

        uint32_t out_w = 0;
        uint16_t out_spp = 0;
        (void)TIFFGetField(wtif, TIFFTAG_IMAGEWIDTH, &out_w);
        (void)TIFFGetField(wtif, TIFFTAG_SAMPLESPERPIXEL, &out_spp);

        TIFFClose(wtif);
    }

    return 0;
}