#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif;
    uint16_t curdir;
    uint16_t ndirs;
    int lastdir;
    int ret;
    uint16_t i;
    uint16_t target;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    (void)TIFFIsBigEndian(tif);
    curdir = TIFFCurrentDirectory(tif);
    (void)curdir;
    lastdir = TIFFLastDirectory(tif);
    (void)lastdir;

    ndirs = TIFFNumberOfDirectories(tif);

    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFCurrentDirectory(tif);
    (void)TIFFLastDirectory(tif);
    (void)TIFFIsBigEndian(tif);

    if (ndirs > 0) {
        for (i = 0; i < ndirs && i < 32; i++) {
            ret = TIFFSetDirectory(tif, i);
            if (ret) {
                (void)TIFFCurrentDirectory(tif);
                (void)TIFFLastDirectory(tif);
                (void)TIFFIsBigEndian(tif);
            }
        }
    }

    if (Size >= 2) {
        target = (uint16_t)((uint16_t)Data[0] | ((uint16_t)Data[1] << 8));
    } else if (Size == 1) {
        target = Data[0];
    } else {
        target = 0;
    }

    (void)TIFFSetDirectory(tif, target);
    (void)TIFFCurrentDirectory(tif);
    (void)TIFFLastDirectory(tif);

    for (i = 0; i < 32; i++) {
        (void)TIFFCurrentDirectory(tif);
        (void)TIFFLastDirectory(tif);
        (void)TIFFIsBigEndian(tif);
        ret = TIFFReadDirectory(tif);
        if (!ret) {
            break;
        }
    }

    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFCurrentDirectory(tif);
    (void)TIFFLastDirectory(tif);
    (void)TIFFNumberOfDirectories(tif);

    TIFFClose(tif);
    return 0;
}