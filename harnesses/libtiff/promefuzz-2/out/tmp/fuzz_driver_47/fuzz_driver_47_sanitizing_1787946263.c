#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint64_t read_u64_le(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    size_t lim = n < 8 ? n : 8;
    for (i = 0; i < lim; i++) {
        v |= ((uint64_t)p[i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    uint64_t diroff = 0;
    uint64_t out_diroff = 0;
    int mode_selector = 0;
    const char *open_mode = "r";
    TIFFTagMethods *tag_methods;

    write_input_file(Data, Size);

    if (Size > 0) {
        mode_selector = Data[0] % 4;
    }

    switch (mode_selector) {
        case 0:
            open_mode = "r";
            break;
        case 1:
            open_mode = "rM";
            break;
        case 2:
            open_mode = "w";
            break;
        default:
            open_mode = "w8";
            break;
    }

    tif = TIFFOpen("./dummy_file", open_mode);
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r+");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w8");
    }
    if (!tif) {
        return 0;
    }

    tag_methods = TIFFAccessTagMethods(tif);
    (void)tag_methods;

    if (Size > 1) {
        diroff = read_u64_le(Data + 1, Size - 1);
    }

    (void)TIFFReadEXIFDirectory(tif, (toff_t)diroff);
    (void)TIFFReadCustomDirectory(tif, (toff_t)diroff, NULL);
    TIFFFreeDirectory(tif);

    if (mode_selector >= 2) {
        (void)TIFFWriteCustomDirectory(tif, &out_diroff);
        (void)TIFFReadEXIFDirectory(tif, (toff_t)out_diroff);
        (void)TIFFReadCustomDirectory(tif, (toff_t)out_diroff, NULL);
        TIFFFreeDirectory(tif);
    }

    if (Size > 16) {
        uint64_t diroff2 = read_u64_le(Data + (Size > 8 ? Size - 8 : 0), Size > 8 ? 8 : Size);
        (void)TIFFReadEXIFDirectory(tif, (toff_t)diroff2);
        (void)TIFFReadCustomDirectory(tif, (toff_t)diroff2, NULL);
        TIFFFreeDirectory(tif);
    }

    TIFFCleanup(tif);
    return 0;
}