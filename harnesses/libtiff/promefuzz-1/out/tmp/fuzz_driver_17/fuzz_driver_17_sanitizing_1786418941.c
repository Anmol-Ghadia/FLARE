#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#ifndef OPEN_FLAGS_RDONLY
#define OPEN_FLAGS_RDONLY (_O_BINARY | _O_RDONLY)
#endif
#ifndef OPEN_FLAGS_RDWR
#define OPEN_FLAGS_RDWR (_O_BINARY | _O_RDWR)
#endif
#ifndef OPEN_FLAGS_CREAT_TRUNC
#define OPEN_FLAGS_CREAT_TRUNC (_O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR)
#endif
#define CLOSE_FD _close
#define OPEN_FD _open
#define WRITE_FD _write
#else
#include <unistd.h>
#ifndef OPEN_FLAGS_RDONLY
#define OPEN_FLAGS_RDONLY O_RDONLY
#endif
#ifndef OPEN_FLAGS_RDWR
#define OPEN_FLAGS_RDWR O_RDWR
#endif
#ifndef OPEN_FLAGS_CREAT_TRUNC
#define OPEN_FLAGS_CREAT_TRUNC (O_CREAT | O_TRUNC | O_RDWR)
#endif
#define CLOSE_FD close
#define OPEN_FD open
#define WRITE_FD write
#endif

#include "tiffio.h"

static uint32_t clamp_dim(uint32_t v) {
    if (v == 0) return 1;
    if (v > 4096) return 4096;
    return v;
}

static void fuzz_tiff_with_mode(const char *mode, int open_flags,
                                uint32_t w, uint32_t h,
                                int orientation, int stop_on_error) {
    int fd;
    TIFF *tif = NULL;
    char emsg[1024];
    TIFFRGBAImage img;
    uint32_t *raster = NULL;
    size_t npixels;
    int began = 0;
    int ok_rgba = 0;

    fd = OPEN_FD("./dummy_file", open_flags);
    if (fd < 0)
        return;

    tif = TIFFFdOpen(fd, "./dummy_file", mode);
    if (tif == NULL) {
        CLOSE_FD(fd);
        return;
    }

    memset(emsg, 0, sizeof(emsg));
    ok_rgba = TIFFRGBAImageOK(tif, emsg);

    memset(&img, 0, sizeof(img));
    if (ok_rgba) {
        began = TIFFRGBAImageBegin(&img, tif, stop_on_error, emsg);
    }

    npixels = (size_t)w * (size_t)h;
    if (npixels == 0 || npixels > (1U << 24)) {
        if (began)
            TIFFRGBAImageEnd(&img);
        TIFFClose(tif);
        return;
    }

    raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
    if (raster != NULL) {
        memset(raster, 0, npixels * sizeof(uint32_t));

        (void)TIFFReadRGBAImageOriented(tif, w, h, raster, orientation, stop_on_error);

        if (ok_rgba) {
            (void)TIFFReadRGBAStripExt(tif, 0, raster, stop_on_error);
            if (h > 1)
                (void)TIFFReadRGBAStripExt(tif, h / 2, raster, stop_on_error);
            if (h > 0)
                (void)TIFFReadRGBAStripExt(tif, h - 1, raster, stop_on_error);

            (void)TIFFReadRGBATileExt(tif, 0, 0, raster, stop_on_error);
            if (w > 1 || h > 1)
                (void)TIFFReadRGBATileExt(tif, w / 2, h / 2, raster, stop_on_error);
            if (w > 0 && h > 0)
                (void)TIFFReadRGBATileExt(tif, w - 1, h - 1, raster, stop_on_error);
        }

        free(raster);
    }

    if (began)
        TIFFRGBAImageEnd(&img);
    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    int fdw;
    uint32_t w, h;
    int orientation;
    int stop_on_error;

    fdw = OPEN_FD("./dummy_file", OPEN_FLAGS_CREAT_TRUNC, 0600);
    if (fdw < 0)
        return 0;

    if (Size > 0) {
        ssize_t written = WRITE_FD(fdw, (const char *)Data, Size);
        (void)written;
    }
    CLOSE_FD(fdw);

    w = 1;
    h = 1;
    orientation = ORIENTATION_TOPLEFT;
    stop_on_error = 0;

    if (Size >= 4) {
        w = clamp_dim(((uint32_t)Data[0] << 8) | Data[1]);
        h = clamp_dim(((uint32_t)Data[2] << 8) | Data[3]);
    }
    if (Size >= 5) {
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
        orientation = orientations[Data[4] % (sizeof(orientations) / sizeof(orientations[0]))];
    }
    if (Size >= 6) {
        stop_on_error = Data[5] & 1;
    }

    fuzz_tiff_with_mode("r", OPEN_FLAGS_RDONLY, w, h, orientation, stop_on_error);
    fuzz_tiff_with_mode("rM", OPEN_FLAGS_RDONLY, w, h, orientation, stop_on_error ^ 1);
    fuzz_tiff_with_mode("r+", OPEN_FLAGS_RDWR, h, w, orientation, stop_on_error);

    return 0;
}