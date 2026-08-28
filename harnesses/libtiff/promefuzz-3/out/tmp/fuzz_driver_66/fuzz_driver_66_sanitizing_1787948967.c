#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 2 > n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 4 > n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    if (off + 8 > n) return 0;
    return (uint64_t)p[off] |
           ((uint64_t)p[off + 1] << 8) |
           ((uint64_t)p[off + 2] << 16) |
           ((uint64_t)p[off + 3] << 24) |
           ((uint64_t)p[off + 4] << 32) |
           ((uint64_t)p[off + 5] << 40) |
           ((uint64_t)p[off + 6] << 48) |
           ((uint64_t)p[off + 7] << 56);
}

static toff_t pick_offset(const uint8_t *Data, size_t Size, size_t selector) {
    switch (selector % 6) {
        case 0:
            return (toff_t)0;
        case 1:
            return (toff_t)8;
        case 2:
            return (toff_t)rd16(Data, Size, 0);
        case 3:
            return (toff_t)rd32(Data, Size, 0);
        case 4:
            return (toff_t)rd64(Data, Size, 0);
        default:
            return (toff_t)(Size > 0 ? (Size - 1) : 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif_rw = NULL;
    TIFF *tif_r = NULL;
    toff_t off1, off2, off3;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    off1 = pick_offset(Data, Size, 0);
    off2 = pick_offset(Data, Size, 1);
    off3 = pick_offset(Data, Size, 2);

    tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r) {
        (void)TIFFSetSubDirectory(tif_r, off1);
        (void)TIFFSetSubDirectory(tif_r, off2);
        (void)TIFFReadEXIFDirectory(tif_r, off1);
        (void)TIFFReadGPSDirectory(tif_r, off2);
        (void)TIFFReadCustomDirectory(tif_r, off3, NULL);
        TIFFClose(tif_r);
        tif_r = NULL;
    }

    tif_rw = TIFFOpen("./dummy_file", "r+");
    if (tif_rw) {
        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFCreateGPSDirectory(tif_rw);

        (void)TIFFReadCustomDirectory(tif_rw, off1, NULL);
        (void)TIFFReadEXIFDirectory(tif_rw, off2);
        (void)TIFFReadGPSDirectory(tif_rw, off3);
        (void)TIFFSetSubDirectory(tif_rw, off1);

        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFCreateEXIFDirectory(tif_rw);

        TIFFClose(tif_rw);
        tif_rw = NULL;
    }

    return 0;
}