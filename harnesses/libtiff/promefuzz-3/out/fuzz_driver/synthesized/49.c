// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentTile at tif_open.c:631:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFComputeStrip at tif_strip.c:36:1 in tiffio.h
// TIFFCurrentRow at tif_open.c:604:1 in tiffio.h
// TIFFCurrentTile at tif_open.c:631:1 in tiffio.h
// TIFFCurrentStrip at tif_open.c:622:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_49(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    (void)TIFFCurrentRow(tif);
    (void)TIFFCurrentTile(tif);
    (void)TIFFCurrentStrip(tif);
    (void)TIFFNumberOfTiles(tif);
    (void)TIFFComputeTile(tif, 0, 0, 0, 0);
    (void)TIFFComputeStrip(tif, 0, 0);

    uint32_t x = 0, y = 0, z = 0, row = 0;
    uint16_t s = 0;

    if (Size >= 2) s = rd16(Data);
    if (Size >= 6) x = rd32(Data + 2);
    if (Size >= 10) y = rd32(Data + 6);
    if (Size >= 14) z = rd32(Data + 10);
    if (Size >= 18) row = rd32(Data + 14);

    (void)TIFFComputeTile(tif, x, y, z, s);
    (void)TIFFComputeTile(tif, y, z, x, (uint16_t)(s ^ 0xFFFFu));
    (void)TIFFComputeTile(tif, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFu);
    (void)TIFFComputeTile(tif, x >> 1, y >> 1, z >> 1, (uint16_t)(s & 0xFFu));

    (void)TIFFNumberOfTiles(tif);

    (void)TIFFComputeStrip(tif, row, s);
    (void)TIFFComputeStrip(tif, x, (uint16_t)(s + 1));
    (void)TIFFComputeStrip(tif, 0xFFFFFFFFu, 0xFFFFu);
    (void)TIFFComputeStrip(tif, y, 0);

    (void)TIFFCurrentRow(tif);
    (void)TIFFCurrentTile(tif);
    (void)TIFFCurrentStrip(tif);

    TIFFClose(tif);
    return 0;
}