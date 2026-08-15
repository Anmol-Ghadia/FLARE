// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsBigEndian at tif_open.c:667:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFIsMSB2LSB at tif_open.c:658:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFReadRGBAStripExt at tif_getimage.c:2921:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
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

#include "tiffio.h"

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

int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
    TIFF *tif;
    TIFFRGBAImage img;
    char emsg[1024];
    uint32_t *raster = NULL;
    uint32_t width = 0, rowsperstrip = 0;
    uint32_t row = 0;
    int stop_on_error = 0;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    (void)TIFFIsBigEndian(tif);
    (void)TIFFIsByteSwapped(tif);
    (void)TIFFIsMSB2LSB(tif);

    memset(&img, 0, sizeof(img));
    memset(emsg, 0, sizeof(emsg));

    if (TIFFRGBAImageBegin(&img, tif, 0, emsg)) {
        (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

        if (width > 0 && width <= (1U << 24)) {
            raster = (uint32_t *)malloc((size_t)width * sizeof(uint32_t));
            if (raster != NULL) {
                if (rowsperstrip == 0) {
                    rowsperstrip = 1;
                }

                if (Size >= 4) {
                    row = ((uint32_t)Data[0] << 24) |
                          ((uint32_t)Data[1] << 16) |
                          ((uint32_t)Data[2] << 8) |
                          (uint32_t)Data[3];
                    row = (row / rowsperstrip) * rowsperstrip;
                } else {
                    row = 0;
                }

                stop_on_error = (Size > 4) ? (Data[4] & 1) : 0;
                (void)TIFFReadRGBAStripExt(tif, row, raster, stop_on_error);

                if (rowsperstrip > 0) {
                    (void)TIFFReadRGBAStripExt(tif, 0, raster, 0);
                    (void)TIFFReadRGBAStripExt(tif, rowsperstrip, raster, 1);
                }
            }
        }
    }

    free(raster);
    TIFFCleanup(tif);
    return 0;
}