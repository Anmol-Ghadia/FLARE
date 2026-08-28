#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    uint64_t lo = rd32(p, n, off);
    uint64_t hi = rd32(p, n, off + 4);
    return lo | (hi << 32);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif;
    uint64_t diroff = 0;
    toff_t off1, off2, off3;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "w8");
    if (tif) {
        (void)TIFFCreateGPSDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff);
        TIFFFreeDirectory(tif);

        if (Size > 0 && (Data[0] & 1)) {
            (void)TIFFCreateGPSDirectory(tif);
            (void)TIFFWriteCustomDirectory(tif, &diroff);
            TIFFFreeDirectory(tif);
        }

        if (Size > 1 && (Data[1] & 1)) {
            (void)TIFFCreateEXIFDirectory(tif);
            (void)TIFFWriteCustomDirectory(tif, &diroff);
            TIFFFreeDirectory(tif);
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        const TIFFFieldArray *gps_fields = _TIFFGetGpsFields();
        const TIFFFieldArray *exif_fields = _TIFFGetExifFields();

        off1 = (toff_t)diroff;
        off2 = (toff_t)rd64(Data, Size, 0);
        off3 = (toff_t)rd32(Data, Size, 8);

        if (gps_fields) {
            (void)TIFFReadCustomDirectory(tif, off1, gps_fields);
            TIFFFreeDirectory(tif);

            (void)TIFFReadCustomDirectory(tif, off2, gps_fields);
            TIFFFreeDirectory(tif);
        }

        if (exif_fields) {
            (void)TIFFReadCustomDirectory(tif, off1, exif_fields);
            TIFFFreeDirectory(tif);

            (void)TIFFReadCustomDirectory(tif, off3, exif_fields);
            TIFFFreeDirectory(tif);
        }

        (void)TIFFCreateGPSDirectory(tif);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        TIFFFreeDirectory(tif);

        TIFFClose(tif);
    }

    remove("./dummy_file");
    return 0;
}