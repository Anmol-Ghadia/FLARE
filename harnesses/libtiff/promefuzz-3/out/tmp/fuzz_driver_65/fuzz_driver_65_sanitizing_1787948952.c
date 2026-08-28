#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

static void write_file(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) (void)fwrite(data, 1, size, fp);
    fclose(fp);
}

static void exercise_read_apis(TIFF *tif, const uint8_t *Data, size_t Size) {
    toff_t off1 = (toff_t)rd64(Data, Size, 0);
    toff_t off2 = (toff_t)rd64(Data, Size, 8);
    toff_t off3 = (toff_t)rd64(Data, Size, 16);
    toff_t off4 = (toff_t)rd64(Data, Size, 24);
    toff_t off5 = (toff_t)rd32(Data, Size, 32);
    toff_t off6 = (toff_t)rd16(Data, Size, 36);

    (void)TIFFSetSubDirectory(tif, off1);
    (void)TIFFReadEXIFDirectory(tif, off2);
    (void)TIFFReadGPSDirectory(tif, off3);
    (void)TIFFReadCustomDirectory(tif, off4, NULL);

    (void)TIFFSetSubDirectory(tif, off5);
    (void)TIFFReadEXIFDirectory(tif, off6);
    (void)TIFFReadGPSDirectory(tif, 0);
    (void)TIFFReadCustomDirectory(tif, 0, NULL);
}

static void exercise_write_apis(TIFF *tif, const uint8_t *Data, size_t Size) {
    toff_t off1 = (toff_t)rd64(Data, Size, 40);
    toff_t off2 = (toff_t)rd64(Data, Size, 48);

    (void)TIFFCreateEXIFDirectory(tif);
    (void)TIFFCreateGPSDirectory(tif);
    (void)TIFFSetSubDirectory(tif, off1);
    (void)TIFFReadEXIFDirectory(tif, off2);
    (void)TIFFReadGPSDirectory(tif, off1);
    (void)TIFFReadCustomDirectory(tif, off2, NULL);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL) {
        exercise_read_apis(tif, Data, Size);
        TIFFClose(tif);
    }

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw != NULL) {
        exercise_write_apis(tifw, Data, Size);
        TIFFClose(tifw);
    }

    TIFF *tifa = TIFFOpen("./dummy_file", "a");
    if (tifa != NULL) {
        (void)TIFFCreateEXIFDirectory(tifa);
        (void)TIFFCreateGPSDirectory(tifa);
        (void)TIFFSetSubDirectory(tifa, (toff_t)rd32(Data, Size, 56));
        TIFFClose(tifa);
    }

    remove("./dummy_file");
    return 0;
}