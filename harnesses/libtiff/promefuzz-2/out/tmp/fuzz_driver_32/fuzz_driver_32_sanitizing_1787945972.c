#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint32_t r = 0, g = 0, b = 0;

    /* Allocate conversion state on heap since TIFFYCbCrToRGB is opaque and
       may be larger than the visible/incomplete declaration permits. */
    {
        TIFFYCbCrToRGB *ycbcr =
            (TIFFYCbCrToRGB *)calloc(1, 4096);
        if (ycbcr != NULL) {
            float luma[3] = {0.299f, 0.587f, 0.114f};
            TIFFYCbCrToRGBInit(ycbcr, luma, NULL);

            {
                uint32_t Y = 0;
                int32_t Cb = 0;
                int32_t Cr = 0;

                if (Size > 0) Y = Data[0];
                if (Size > 1) Cb = Data[1];
                if (Size > 2) Cr = Data[2];
                TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);

                if (Size >= 6) {
                    Y = Data[3];
                    Cb = Data[4];
                    Cr = Data[5];
                    TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);
                }

                if (Size >= 9) {
                    Y = (uint32_t)(Data[6] ^ 0xFFu);
                    Cb = Data[7];
                    Cr = Data[8];
                    TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);
                }
            }

            free(ycbcr);
        }
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0) {
                (void)fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }

        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif != NULL) {
            uint32_t width = 0, height = 0;
            uint32_t tilew = 0, tileh = 0;
            int tiled = TIFFIsTiled(tif);

            (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
            (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
            (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew);
            (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh);

            if (width == 0) width = (Size >= 4) ? (rd32(Data) & 0x3FFu) : 1u;
            if (height == 0) height = (Size >= 8) ? (rd32(Data + 4) & 0x3FFu) : 1u;
            if (width == 0) width = 1u;
            if (height == 0) height = 1u;

            {
                size_t pixels = (size_t)width * (size_t)height;
                if (pixels == 0) pixels = 1;
                if (pixels > (1u << 24)) pixels = (1u << 24);

                uint32_t *raster = (uint32_t *)malloc(pixels * sizeof(uint32_t));
                if (raster != NULL) {
                    memset(raster, 0, pixels * sizeof(uint32_t));

                    (void)TIFFReadRGBAImage(tif, width, height, raster,
                                            (Size > 0) ? (Data[0] & 1) : 0);

                    {
                        int orientation = ORIENTATION_BOTLEFT;
                        if (Size > 1) {
                            switch (Data[1] % 8) {
                                case 0: orientation = ORIENTATION_TOPLEFT; break;
                                case 1: orientation = ORIENTATION_TOPRIGHT; break;
                                case 2: orientation = ORIENTATION_BOTRIGHT; break;
                                case 3: orientation = ORIENTATION_BOTLEFT; break;
                                case 4: orientation = ORIENTATION_LEFTTOP; break;
                                case 5: orientation = ORIENTATION_RIGHTTOP; break;
                                case 6: orientation = ORIENTATION_RIGHTBOT; break;
                                default: orientation = ORIENTATION_LEFTBOT; break;
                            }
                        }
                        (void)TIFFReadRGBAImageOriented(
                            tif, width, height, raster, orientation,
                            (Size > 2) ? (Data[2] & 1) : 0);
                    }

                    free(raster);
                }
            }

            if (tiled) {
                uint32_t use_tilew = tilew ? tilew : 1u;
                uint32_t use_tileh = tileh ? tileh : 1u;
                size_t tile_pixels = (size_t)use_tilew * (size_t)use_tileh;
                if (tile_pixels == 0) tile_pixels = 1;
                if (tile_pixels > (1u << 24)) tile_pixels = (1u << 24);

                uint32_t *tilebuf = (uint32_t *)malloc(tile_pixels * sizeof(uint32_t));
                if (tilebuf != NULL) {
                    uint32_t col = 0, row = 0;

                    memset(tilebuf, 0, tile_pixels * sizeof(uint32_t));

                    if (Size >= 16) {
                        col = rd32(Data + 8);
                        row = rd32(Data + 12);
                    }

                    if (use_tilew != 0) col = (col / use_tilew) * use_tilew;
                    if (use_tileh != 0) row = (row / use_tileh) * use_tileh;

                    (void)TIFFReadRGBATile(tif, col, row, tilebuf);
                    (void)TIFFReadRGBATileExt(
                        tif, col, row, tilebuf,
                        (Size > 16) ? (Data[16] & 1) : 0);

                    if (width > 0 && height > 0) {
                        uint32_t alt_col = 0;
                        uint32_t alt_row = 0;
                        if (use_tilew != 0 && width > use_tilew)
                            alt_col = ((width - 1) / use_tilew) * use_tilew;
                        if (use_tileh != 0 && height > use_tileh)
                            alt_row = ((height - 1) / use_tileh) * use_tileh;

                        (void)TIFFReadRGBATile(tif, alt_col, alt_row, tilebuf);
                        (void)TIFFReadRGBATileExt(
                            tif, alt_col, alt_row, tilebuf,
                            (Size > 17) ? (Data[17] & 1) : 1);
                    }

                    free(tilebuf);
                }
            } else {
                uint32_t dummy_tile[1] = {0};
                (void)TIFFReadRGBATile(tif, 0, 0, dummy_tile);
                (void)TIFFReadRGBATileExt(
                    tif, 0, 0, dummy_tile,
                    (Size > 18) ? (Data[18] & 1) : 0);
            }

            {
                TIFFRGBAImage *img = (TIFFRGBAImage *)calloc(1, sizeof(TIFFRGBAImage));
                if (img != NULL) {
                    img->tif = tif;
                    TIFFRGBAImageEnd(img);
                    free(img);
                }
            }

            TIFFClose(tif);
        }
    }

    return 0;
}