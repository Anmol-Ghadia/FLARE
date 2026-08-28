// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBAStrip at tif_getimage.c:2914:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsTiled at tif_open.c:595:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFRGBAImageGet at tif_getimage.c:501:1 in tiffio.h
// TIFFRGBAImageGet at tif_getimage.c:501:1 in tiffio.h
// TIFFRGBAImageEnd at tif_getimage.c:215:1 in tiffio.h
// TIFFReadRGBAStrip at tif_getimage.c:2914:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
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

#include "tiffio.h"

static uint32_t safe_mul_u32(uint32_t a, uint32_t b) {
    if (a == 0 || b == 0) return 0;
    if (a > 0xFFFFFFFFu / b) return 0;
    return a * b;
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    do {
        uint32_t width = 0, height = 0;
        uint32_t rowsperstrip = 0;
        uint32_t tilewidth = 0, tilelength = 0;
        int is_tiled = TIFFIsTiled(tif);

        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
        (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tilelength);

        {
            TIFFRGBAImage img;
            char emsg[1024];
            memset(&img, 0, sizeof(img));
            memset(emsg, 0, sizeof(emsg));

            if (TIFFRGBAImageBegin(&img, tif, (Size > 0) ? (Data[0] & 1) : 0, emsg)) {
                uint32_t w = img.width;
                uint32_t h = img.height;
                uint32_t npixels = safe_mul_u32(w, h);

                if (npixels > 0 && npixels <= (1U << 26)) {
                    uint32_t *raster = (uint32_t *)malloc((size_t)npixels * sizeof(uint32_t));
                    if (raster) {
                        memset(raster, 0, (size_t)npixels * sizeof(uint32_t));
                        (void)TIFFRGBAImageGet(&img, raster, w, h);

                        if (w > 0 && h > 0) {
                            uint32_t altw = (Size > 1) ? ((uint32_t)Data[1] % (w + 1)) : w;
                            uint32_t alth = (Size > 2) ? ((uint32_t)Data[2] % (h + 1)) : h;
                            if (altw > 0 && alth > 0 &&
                                safe_mul_u32(altw, alth) <= npixels) {
                                (void)TIFFRGBAImageGet(&img, raster, altw, alth);
                            }
                        }

                        free(raster);
                    }
                }

                TIFFRGBAImageEnd(&img);
            }
        }

        if (!is_tiled) {
            uint32_t rps = rowsperstrip;
            if (rps == 0 || rps == 0xFFFFFFFFu)
                rps = (height > 0) ? height : 1;

            {
                uint32_t strip_h = (rps > 0 && height > 0 && rps < height) ? rps : ((height > 0) ? height : 1);
                uint32_t npixels = safe_mul_u32(width ? width : 1, strip_h ? strip_h : 1);
                if (npixels > 0 && npixels <= (1U << 26)) {
                    uint32_t *raster = (uint32_t *)malloc((size_t)npixels * sizeof(uint32_t));
                    if (raster) {
                        uint32_t row = 0;
                        int stop_on_error = (Size > 3) ? (Data[3] & 1) : 0;
                        memset(raster, 0, (size_t)npixels * sizeof(uint32_t));

                        (void)TIFFReadRGBAStrip(tif, row, raster);
                        (void)TIFFReadRGBAStripExt(tif, row, raster, stop_on_error);

                        if (height > 0 && rps > 0) {
                            uint32_t aligned_row = ((Size > 4) ? ((uint32_t)Data[4] % ((height + rps - 1) / rps)) : 0) * rps;
                            if (aligned_row < height) {
                                (void)TIFFReadRGBAStrip(tif, aligned_row, raster);
                                (void)TIFFReadRGBAStripExt(tif, aligned_row, raster, !stop_on_error);
                            }
                        }

                        free(raster);
                    }
                }
            }
        } else {
            uint32_t tw = tilewidth ? tilewidth : 1;
            uint32_t th = tilelength ? tilelength : 1;
            uint32_t npixels = safe_mul_u32(tw, th);

            if (npixels > 0 && npixels <= (1U << 26)) {
                uint32_t *raster = (uint32_t *)malloc((size_t)npixels * sizeof(uint32_t));
                if (raster) {
                    uint32_t col = 0, row = 0;
                    int stop_on_error = (Size > 5) ? (Data[5] & 1) : 0;
                    memset(raster, 0, (size_t)npixels * sizeof(uint32_t));

                    (void)TIFFReadRGBATileExt(tif, col, row, raster, stop_on_error);

                    if (width > 0 && height > 0) {
                        uint32_t max_tile_x = (width + tw - 1) / tw;
                        uint32_t max_tile_y = (height + th - 1) / th;
                        uint32_t tx = (Size > 6 && max_tile_x > 0) ? ((uint32_t)Data[6] % max_tile_x) : 0;
                        uint32_t ty = (Size > 7 && max_tile_y > 0) ? ((uint32_t)Data[7] % max_tile_y) : 0;
                        col = tx * tw;
                        row = ty * th;
                        (void)TIFFReadRGBATileExt(tif, col, row, raster, !stop_on_error);
                    }

                    free(raster);
                }
            }
        }
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}