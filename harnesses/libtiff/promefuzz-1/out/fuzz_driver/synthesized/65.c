// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFRGBAImageEnd at tif_getimage.c:215:1 in tiffio.h
// TIFFReadRGBAImageOriented at tif_getimage.c:520:1 in tiffio.h
// TIFFReadRGBAImage at tif_getimage.c:546:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsUpSampled at tif_open.c:649:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFRGBAImageGet at tif_getimage.c:501:1 in tiffio.h
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
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint32_t bounded_dim(uint32_t v) {
    return (v % 64U) + 1U;
}

int LLVMFuzzerTestOneInput_65(const uint8_t *Data, size_t Size) {
    char emsg[1024];
    TIFF *tif = NULL;
    TIFFRGBAImage img;
    uint32_t *raster = NULL;
    uint32_t w = 1, h = 1;
    size_t npixels;
    int stop = 0;
    int orientation = ORIENTATION_TOPLEFT;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    (void)TIFFIsUpSampled(tif);

    memset(emsg, 0, sizeof(emsg));
    (void)TIFFRGBAImageOK(tif, emsg);

    memset(&img, 0, sizeof(img));
    stop = (Size > 0) ? (Data[0] & 1) : 0;
    if (TIFFRGBAImageBegin(&img, tif, stop, emsg)) {
        uint32_t candidates[6];
        size_t i;

        candidates[0] = img.width ? img.width : 1U;
        candidates[1] = img.height ? img.height : 1U;
        candidates[2] = (Size > 1) ? bounded_dim(Data[1]) : 1U;
        candidates[3] = (Size > 2) ? bounded_dim(Data[2]) : 1U;
        candidates[4] = (img.width && img.height) ? (img.width < 64U ? img.width : 64U) : 1U;
        candidates[5] = (img.width && img.height) ? (img.height < 64U ? img.height : 64U) : 1U;

        w = candidates[0];
        h = candidates[1];
        if (w == 0) w = 1;
        if (h == 0) h = 1;
        if (w > 4096U) w = 4096U;
        if (h > 4096U) h = 4096U;

        npixels = (size_t)w * (size_t)h;
        if (npixels != 0 && npixels <= (1U << 22)) {
            raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
            if (raster) {
                memset(raster, 0, npixels * sizeof(uint32_t));
                (void)TIFFRGBAImageGet(&img, raster, w, h);
                free(raster);
                raster = NULL;
            }
        }

        for (i = 0; i < 3; i++) {
            uint32_t tw = candidates[(i * 2) % 6];
            uint32_t th = candidates[(i * 2 + 1) % 6];
            if (tw == 0) tw = 1;
            if (th == 0) th = 1;
            if (tw > 128U) tw = 128U;
            if (th > 128U) th = 128U;
            npixels = (size_t)tw * (size_t)th;
            if (npixels == 0 || npixels > (1U << 20)) {
                continue;
            }
            raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
            if (!raster) {
                continue;
            }
            memset(raster, 0, npixels * sizeof(uint32_t));

            orientation = (Size > 3)
                              ? ((Data[3] % 8) + 1)
                              : ORIENTATION_BOTLEFT;
            stop = (Size > 4) ? (Data[4] & 1) : 0;

            (void)TIFFReadRGBAImageOriented(tif, tw, th, raster, orientation, stop);
            (void)TIFFReadRGBAImage(tif, tw, th, raster, stop);

            free(raster);
            raster = NULL;
        }

        TIFFRGBAImageEnd(&img);
    } else {
        uint32_t tw = (Size > 5) ? bounded_dim(Data[5]) : 1U;
        uint32_t th = (Size > 6) ? bounded_dim(Data[6]) : 1U;
        npixels = (size_t)tw * (size_t)th;
        if (npixels != 0 && npixels <= (1U << 18)) {
            raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
            if (raster) {
                memset(raster, 0, npixels * sizeof(uint32_t));
                orientation = (Size > 7) ? ((Data[7] % 8) + 1) : ORIENTATION_TOPLEFT;
                stop = (Size > 8) ? (Data[8] & 1) : 0;
                (void)TIFFReadRGBAImageOriented(tif, tw, th, raster, orientation, stop);
                (void)TIFFReadRGBAImage(tif, tw, th, raster, stop);
                free(raster);
                raster = NULL;
            }
        }
    }

    TIFFClose(tif);
    return 0;
}