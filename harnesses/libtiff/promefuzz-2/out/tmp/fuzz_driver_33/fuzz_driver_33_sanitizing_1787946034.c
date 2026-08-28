#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint32_t r = 0, g = 0, b = 0;

    if (Size >= 6) {
        TIFFYCbCrToRGB *ycbcr = (TIFFYCbCrToRGB *)calloc(1, sizeof(TIFFYCbCrToRGB));
        if (ycbcr) {
            float luma[3];
            float refbw[6];

            luma[0] = (float)(Data[0] ? Data[0] : 1) / 255.0f;
            luma[1] = (float)(Data[1] ? Data[1] : 1) / 255.0f;
            luma[2] = (float)(Data[2] ? Data[2] : 1) / 255.0f;
            refbw[0] = 0.0f;
            refbw[1] = 255.0f;
            refbw[2] = 0.0f;
            refbw[3] = 255.0f;
            refbw[4] = 0.0f;
            refbw[5] = 255.0f;

            if (TIFFYCbCrToRGBInit(ycbcr, luma, refbw)) {
                uint32_t Y = Data[3];
                int32_t Cb = (int32_t)(int8_t)Data[4];
                int32_t Cr = (int32_t)(int8_t)Data[5];
                TIFFYCbCrtoRGB(ycbcr, Y, Cb, Cr, &r, &g, &b);
                TIFFYCbCrtoRGB(ycbcr, 255U - Y, -Cb, -Cr, &r, &g, &b);
                TIFFYCbCrtoRGB(ycbcr, (uint32_t)(Y ^ 0xFFU), Cb + 128, Cr - 128, &r, &g, &b);
            }
            free(ycbcr);
        }
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (!fp) {
            return 0;
        }
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    {
        uint32_t width = 0, height = 0;
        uint32_t tilew = 0, tileh = 0;
        uint16_t orientation = ORIENTATION_BOTLEFT;
        char emsg[1024];

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh);

        if (width == 0) width = (Size >= 4) ? (uint32_t)rd16(Data) + 1U : 1U;
        if (height == 0) height = (Size >= 6) ? (uint32_t)rd16(Data + 2) + 1U : 1U;

        if (width > 4096U) width = 4096U;
        if (height > 4096U) height = 4096U;

        {
            size_t npixels = (size_t)width * (size_t)height;
            if (npixels > 0 && npixels <= (1U << 24)) {
                uint32_t *raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
                if (raster) {
                    (void)TIFFReadRGBAImage(tif, width, height, raster, (Size > 0) ? (Data[0] & 1) : 0);
                    (void)TIFFReadRGBAImageOriented(
                        tif, width, height, raster,
                        (Size > 1) ? (int)(Data[1] % 9) : (int)orientation,
                        (Size > 2) ? (int)(Data[2] & 1) : 0);
                    (void)TIFFRGBAImageOK(tif, emsg);
                    free(raster);
                }
            }
        }

        if (TIFFIsTiled(tif)) {
            if (tilew == 0) tilew = 1;
            if (tileh == 0) tileh = 1;

            {
                size_t tile_pixels = (size_t)tilew * (size_t)tileh;
                if (tile_pixels > 0 && tile_pixels <= (1U << 24)) {
                    uint32_t *tilebuf = (uint32_t *)malloc(tile_pixels * sizeof(uint32_t));
                    if (tilebuf) {
                        uint32_t col = 0, row = 0;

                        if (Size >= 14) {
                            col = rd32(Data + 6);
                            row = rd32(Data + 10);
                        }

                        col = (col / tilew) * tilew;
                        row = (row / tileh) * tileh;

                        (void)TIFFReadRGBATile(tif, col, row, tilebuf);
                        (void)TIFFReadRGBATileExt(tif, col, row, tilebuf, (Size > 3) ? (Data[3] & 1) : 0);

                        if (width > 0 && height > 0) {
                            uint32_t last_col = ((width - 1U) / tilew) * tilew;
                            uint32_t last_row = ((height - 1U) / tileh) * tileh;
                            (void)TIFFReadRGBATile(tif, 0, 0, tilebuf);
                            (void)TIFFReadRGBATileExt(tif, last_col, last_row, tilebuf,
                                                      (Size > 4) ? (Data[4] & 1) : 1);
                        }

                        free(tilebuf);
                    }
                }
            }
        }

        {
            TIFFRGBAImage img;
            memset(&img, 0, sizeof(img));
            TIFFRGBAImageEnd(&img);
        }
    }

    TIFFClose(tif);
    return 0;
}