#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <tiffio.h>

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t rd64(const uint8_t *p) {
    return (uint64_t)rd32(p) | ((uint64_t)rd32(p + 4) << 32);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) tif = TIFFOpen("./dummy_file", "r");
    if (!tif) tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return 0;

    TIFFTagMethods *methods = TIFFAccessTagMethods(tif);
    (void)methods;

    uint64_t written_dir_off = 0;
    (void)TIFFWriteCustomDirectory(tif, &written_dir_off);

    toff_t offsets[5];
    offsets[0] = 0;
    offsets[1] = (toff_t)written_dir_off;
    offsets[2] = 8;
    offsets[3] = (toff_t)(Size >= 8 ? rd64(Data) : (Size >= 4 ? rd32(Data) : 0));
    offsets[4] = (toff_t)(Size >= 2 ? rd16(Data) : 0);

    for (size_t i = 0; i < 5; ++i) {
        (void)TIFFReadEXIFDirectory(tif, offsets[i]);
    }

    for (size_t i = 0; i < 5; ++i) {
        (void)TIFFReadCustomDirectory(tif, offsets[i], NULL);
    }

    TIFFFreeDirectory(tif);
    TIFFCleanup(tif);
    return 0;
}