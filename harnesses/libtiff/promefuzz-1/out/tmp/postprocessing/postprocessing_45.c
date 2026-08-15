#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tiffio.h>

static uint32_t clamp_u32_from_size(size_t v) {
    if (v > 0xFFFFFFFFu) return 0xFFFFFFFFu;
    return (uint32_t)v;
}

static uint32_t pick_u32(const uint8_t *Data, size_t Size, size_t off, uint32_t defval) {
    if (off + 4 <= Size) {
        return ((uint32_t)Data[off]) |
               ((uint32_t)Data[off + 1] << 8) |
               ((uint32_t)Data[off + 2] << 16) |
               ((uint32_t)Data[off + 3] << 24);
    }
    return defval;
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
        remove("./dummy_file");
        return 0;
    }

    char emsg[1024];
    memset(emsg, 0, sizeof(emsg));

    (void)TIFFRGBAImageOK(tif, emsg);

    TIFFRGBAImage img;
    memset(&img, 0, sizeof(img));
    (void)TIFFRGBAImageBegin(&img, tif, (Size > 0) ? (Data[0] & 1) : 0, emsg);

    uint32_t width = 0, height = 0;
    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    if (width == 0) width = (pick_u32(Data, Size, 1, 1) % 64) + 1;
    if (height == 0) height = (pick_u32(Data, Size, 5, 1) % 64) + 1;

    {
        size_t npixels = (size_t)width * (size_t)height;
        if (npixels > 0 && npixels <= (1U << 24)) {
            uint32_t *raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
            if (raster) {
                memset(raster, 0, npixels * sizeof(uint32_t));

                (void)TIFFReadRGBAImage(tif, width, height, raster,
                                        (Size > 1) ? (Data[1] & 1) : 0);

                (void)TIFFReadRGBAImageOriented(
                    tif, width, height, raster,
                    (Size > 2) ? (int)(Data[2] % 9) : ORIENTATION_BOTLEFT,
                    (Size > 3) ? (int)(Data[3] & 1) : 0);

                free(raster);
            }
        }
    }

    {
        uint32_t tilew = 0, tileh = 0;
        if (TIFFIsTiled(tif)) {
            (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew);
            (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh);

            if (tilew == 0) tilew = 1;
            if (tileh == 0) tileh = 1;

            size_t npixels = (size_t)tilew * (size_t)tileh;
            if (npixels > 0 && npixels <= (1U << 24)) {
                uint32_t *tilebuf = (uint32_t *)malloc(npixels * sizeof(uint32_t));
                if (tilebuf) {
                    memset(tilebuf, 0, npixels * sizeof(uint32_t));

                    uint32_t col = 0;
                    uint32_t row = 0;
                    if (Size >= 12) {
                        col = pick_u32(Data, Size, 8, 0);
                        row = pick_u32(Data, Size, 12, 0);
                    }
                    if (tilew != 0) col = (col / tilew) * tilew;
                    if (tileh != 0) row = (row / tileh) * tileh;

                    (void)TIFFReadRGBATile(tif, col, row, tilebuf);

                    if (width > 0 && height > 0) {
                        (void)TIFFReadRGBATile(tif,
                                               (width > tilew) ? (width - tilew) : 0,
                                               (height > tileh) ? (height - tileh) : 0,
                                               tilebuf);
                    }

                    free(tilebuf);
                }
            }
        } else {
            uint32_t rowsperstrip = 0;
            (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
            if (rowsperstrip == 0 || rowsperstrip == 0xFFFFFFFFu) {
                rowsperstrip = height ? height : 1;
            }

            size_t strip_pixels = (size_t)width * (size_t)rowsperstrip;
            if (strip_pixels > 0 && strip_pixels <= (1U << 24)) {
                uint32_t *stripbuf = (uint32_t *)malloc(strip_pixels * sizeof(uint32_t));
                if (stripbuf) {
                    memset(stripbuf, 0, strip_pixels * sizeof(uint32_t));

                    uint32_t row = 0;
                    if (Size >= 16 && height > 0) {
                        row = pick_u32(Data, Size, 8, 0) % height;
                    }

                    (void)TIFFReadRGBAStrip(tif, row, stripbuf);

                    if (height > 0) {
                        (void)TIFFReadRGBAStrip(tif, 0, stripbuf);
                        (void)TIFFReadRGBAStrip(tif,
                                                (height > rowsperstrip) ? (height - rowsperstrip) : 0,
                                                stripbuf);
                    }

                    free(stripbuf);
                }
            }
        }
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}