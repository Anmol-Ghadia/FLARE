#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <tiffio.h>

static uint16_t read_u16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t read_u32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
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

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    uint16_t dir_seed1 = read_u16(Data, Size, 0);
    uint16_t dir_seed2 = read_u16(Data, Size, 2);
    uint32_t row_seed1 = read_u32(Data, Size, 4);
    uint32_t row_seed2 = read_u32(Data, Size, 8);
    uint32_t col_seed1 = read_u32(Data, Size, 12);
    uint16_t sample_seed = read_u16(Data, Size, 16);

    uint32_t width = 0, length = 0;
    uint32_t tilewidth = 0, tilelength = 0;
    uint16_t samplesperpixel = 1;
    uint16_t bitspersample = 8;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &length);
    (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);
    (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
    (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);

    {
        uint16_t dirs[4];
        dirs[0] = 0;
        dirs[1] = dir_seed1;
        dirs[2] = dir_seed2;
        dirs[3] = (uint16_t)(dir_seed1 ^ dir_seed2);

        for (size_t i = 0; i < 4; i++) {
            if (TIFFSetDirectory(tif, dirs[i])) {
                (void)TIFFCurrentRow(tif);
                (void)TIFFCurrentTile(tif);

                uint32_t local_width = 0, local_length = 0;
                uint32_t local_tilewidth = 0, local_tilelength = 0;
                uint16_t local_spp = samplesperpixel;
                uint16_t local_bps = bitspersample;

                (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &local_width);
                (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &local_length);
                (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &local_tilewidth);
                (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &local_tilelength);
                (void)TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &local_spp);
                (void)TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &local_bps);

                tmsize_t scanline = TIFFScanlineSize(tif);
                if (scanline > 0 && scanline < (tmsize_t)(1 << 24)) {
                    void *scanbuf = malloc((size_t)scanline);
                    if (scanbuf) {
                        memset(scanbuf, 0, (size_t)scanline);

                        uint32_t rows[3];
                        rows[0] = 0;
                        rows[1] = local_length ? (row_seed1 % local_length) : row_seed1;
                        rows[2] = local_length ? (row_seed2 % local_length) : row_seed2;

                        uint16_t sample = local_spp ? (uint16_t)(sample_seed % local_spp) : 0;

                        for (size_t r = 0; r < 3; r++) {
                            (void)TIFFReadScanline(tif, scanbuf, rows[r], sample);
                            (void)TIFFCurrentRow(tif);
                        }
                        free(scanbuf);
                    }
                }

                {
                    uint32_t rgba_w = local_width ? local_width : width;
                    if (rgba_w == 0) rgba_w = 1;
                    if (rgba_w < (1U << 24)) {
                        uint32_t *raster = (uint32_t *)malloc((size_t)rgba_w * sizeof(uint32_t));
                        if (raster) {
                            memset(raster, 0, (size_t)rgba_w * sizeof(uint32_t));

                            uint32_t strip_rows[3];
                            strip_rows[0] = 0;
                            strip_rows[1] = local_length ? (row_seed1 % local_length) : row_seed1;
                            strip_rows[2] = local_length ? (row_seed2 % local_length) : row_seed2;

                            for (size_t r = 0; r < 3; r++) {
                                (void)TIFFReadRGBAStrip(tif, strip_rows[r], raster);
                                (void)TIFFCurrentRow(tif);
                            }
                            free(raster);
                        }
                    }
                }

                if (TIFFIsTiled(tif)) {
                    uint32_t tw = local_tilewidth ? local_tilewidth : tilewidth;
                    uint32_t tl = local_tilelength ? local_tilelength : tilelength;

                    if (tw == 0) tw = 1;
                    if (tl == 0) tl = 1;

                    if (tw < (1U << 16) && tl < (1U << 16)) {
                        size_t npixels = (size_t)tw * (size_t)tl;
                        if (npixels > 0 && npixels < (size_t)(1 << 24)) {
                            uint32_t *tilebuf = (uint32_t *)malloc(npixels * sizeof(uint32_t));
                            if (tilebuf) {
                                memset(tilebuf, 0, npixels * sizeof(uint32_t));

                                uint32_t cols[3];
                                uint32_t rows[3];

                                cols[0] = 0;
                                cols[1] = tw ? (col_seed1 / tw) * tw : col_seed1;
                                cols[2] = width && tw ? ((col_seed1 % width) / tw) * tw : col_seed1;

                                rows[0] = 0;
                                rows[1] = tl ? (row_seed1 / tl) * tl : row_seed1;
                                rows[2] = length && tl ? ((row_seed2 % length) / tl) * tl : row_seed2;

                                for (size_t t = 0; t < 3; t++) {
                                    (void)TIFFReadRGBATile(tif, cols[t], rows[t], tilebuf);
                                    (void)TIFFCurrentTile(tif);
                                }
                                free(tilebuf);
                            }
                        }
                    }
                }
            } else {
                (void)TIFFCurrentRow(tif);
                (void)TIFFCurrentTile(tif);
            }
        }
    }

    TIFFClose(tif);
    return 0;
}