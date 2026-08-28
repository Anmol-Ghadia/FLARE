#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint32_t clamp_dim(uint32_t v) {
    if (v == 0) return 1;
    if (v > 4096) return 4096;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "rm");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
        if (!tif) {
            TIFFRGBAImage img;
            memset(&img, 0, sizeof(img));
            TIFFRGBAImageEnd(&img);
            return 0;
        }
    }

    do {
        uint32_t width = 0, height = 0, rowsperstrip = 0;
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

        width = clamp_dim(width);
        height = clamp_dim(height);

        size_t npixels = (size_t)width * (size_t)height;
        if (npixels == 0 || npixels > (1U << 24)) {
            npixels = 1;
            width = 1;
            height = 1;
        }

        uint32_t *raster = (uint32_t *)_TIFFmalloc(npixels * sizeof(uint32_t));
        if (!raster) {
            break;
        }
        memset(raster, 0, npixels * sizeof(uint32_t));

        int stop_flag = (Size > 0) ? (Data[0] & 1) : 0;
        int orientation = ORIENTATION_BOTLEFT;
        if (Size > 1) {
            static const int orientations[] = {
                ORIENTATION_TOPLEFT,
                ORIENTATION_TOPRIGHT,
                ORIENTATION_BOTRIGHT,
                ORIENTATION_BOTLEFT,
                ORIENTATION_LEFTTOP,
                ORIENTATION_RIGHTTOP,
                ORIENTATION_RIGHTBOT,
                ORIENTATION_LEFTBOT
            };
            orientation = orientations[Data[1] % (sizeof(orientations) / sizeof(orientations[0]))];
        }

        (void)TIFFReadRGBAImage(tif, width, height, raster, stop_flag);
        (void)TIFFReadRGBAImageOriented(tif, width, height, raster, orientation, stop_flag);
        (void)TIFFReadRGBAImageOriented(tif, width, height, raster, ORIENTATION_BOTLEFT, !stop_flag);

        if (rowsperstrip != 0 && (TIFFIsTiled(tif) == 0)) {
            uint32_t row = 0;
            if (height > 0) {
                if (Size > 2) {
                    uint32_t strip_index = (uint32_t)Data[2];
                    row = strip_index * rowsperstrip;
                    if (row >= height) {
                        row = 0;
                    }
                    if (rowsperstrip != 0) {
                        row = (row / rowsperstrip) * rowsperstrip;
                    }
                }
                (void)TIFFReadRGBAStripExt(tif, row, raster, stop_flag);
                (void)TIFFReadRGBAStripExt(tif, 0, raster, !stop_flag);
            }
        }

        {
            TIFFRGBAImage img;
            char emsg[1024];
            memset(&img, 0, sizeof(img));
            memset(emsg, 0, sizeof(emsg));

            if (TIFFRGBAImageBegin(&img, tif, stop_flag, emsg)) {
                uint32_t gw = width;
                uint32_t gh = height;
                if (Size > 4) {
                    gw = clamp_dim((uint32_t)Data[3] + 1);
                    gh = clamp_dim((uint32_t)Data[4] + 1);
                }
                (void)TIFFRGBAImageGet(&img, raster, gw, gh);
                (void)TIFFRGBAImageGet(&img, raster, width, height);
            }

            TIFFRGBAImageEnd(&img);
            TIFFRGBAImageEnd(&img);
        }

        _TIFFfree(raster);
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}