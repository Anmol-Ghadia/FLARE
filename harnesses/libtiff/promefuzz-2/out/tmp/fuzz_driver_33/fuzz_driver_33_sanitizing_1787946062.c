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

    {
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

            if (width == 0) width = (Size >= 2) ? (uint32_t)rd16(Data) + 1U : 1U;
            if (height == 0) height = (Size >= 4) ? (uint32_t)rd16(Data + 2) + 1U : 1U;

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

                            if (Size >= 12) {
                                col = rd32(Data + 4);
                                row = rd32(Data + 8);
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
    }

    return 0;
}