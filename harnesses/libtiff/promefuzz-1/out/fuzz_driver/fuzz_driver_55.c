// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFCurrentDirOffset at tif_dir.c:1715:1 in tiffio.h
// TIFFGetStrileOffset at tif_dirread.c:6390:10 in tiffio.h
// TIFFGetStrileOffsetWithErr at tif_dirread.c:6396:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFGetStrileOffset at tif_dirread.c:6390:10 in tiffio.h
// TIFFGetStrileOffsetWithErr at tif_dirread.c:6396:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFComputeTile at tif_tile.c:36:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f) return;
    if (Size > 0) (void)fwrite(Data, 1, Size, f);
    fclose(f);
}

static void exercise_tiff(TIFF *tif, const uint8_t *Data, size_t Size) {
    uint64_t dirOff;
    int err = 0;
    uint32_t strile = 0;
    uint32_t x = 0, y = 0, z = 0;
    uint16_t s = 0;
    uint32_t i, limit;

    if (!tif) return;

    dirOff = TIFFCurrentDirOffset(tif);
    (void)dirOff;

    if (Size >= 4) strile = rd32(Data);
    if (Size >= 8) x = rd32(Data + 4);
    if (Size >= 12) y = rd32(Data + 8);
    if (Size >= 16) z = rd32(Data + 12);
    if (Size >= 18) s = rd16(Data + 16);

    (void)TIFFGetStrileOffset(tif, strile);
    err = 0;
    (void)TIFFGetStrileOffsetWithErr(tif, strile, &err);
    err = 0;
    (void)TIFFGetStrileByteCountWithErr(tif, strile, &err);
    (void)TIFFComputeTile(tif, x, y, z, s);

    limit = (uint32_t)((Size < 64) ? Size : 64);
    for (i = 0; i < limit; i++) {
        uint32_t idx = (Size >= 4) ? (rd32(Data + (i % (Size >= 4 ? (Size - 3) : 1))) ^ i) : i;
        uint32_t lx = (Size > 0) ? Data[i % Size] : i;
        uint32_t ly = (Size > 1) ? Data[(i + 1) % Size] : (i * 3U);
        uint32_t lz = (Size > 2) ? Data[(i + 2) % Size] : (i * 7U);
        uint16_t ls = (Size > 3) ? (uint16_t)Data[(i + 3) % Size] : (uint16_t)i;

        err = 0;
        (void)TIFFGetStrileOffset(tif, idx);
        (void)TIFFGetStrileOffsetWithErr(tif, idx, &err);
        err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, idx, &err);
        (void)TIFFComputeTile(tif, lx, ly, lz, ls);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    TIFF *tifw = NULL;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        exercise_tiff(tif, Data, Size);

        if (TIFFReadDirectory(tif)) {
            exercise_tiff(tif, Data, Size);
        }

        TIFFClose(tif);
    }

    tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        uint32_t width = 1, height = 1, tilew = 16, tileh = 16;
        uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
        uint16_t planar = PLANARCONFIG_CONTIG;
        uint16_t samplefmt = SAMPLEFORMAT_UINT;
        uint16_t compression = COMPRESSION_NONE;

        if (Size >= 4) width = (rd32(Data) % 128U) + 1U;
        if (Size >= 8) height = (rd32(Data + 4) % 128U) + 1U;
        if (Size >= 12) tilew = (rd32(Data + 8) % 64U) + 1U;
        if (Size >= 16) tileh = (rd32(Data + 12) % 64U) + 1U;
        if (Size >= 17) spp = (uint16_t)((Data[16] % 4U) + 1U);
        if (Size >= 18) bps = (Data[17] & 1U) ? 16 : 8;
        if (Size >= 19) planar = (Data[18] & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;

        (void)TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tifw, TIFFTAG_SAMPLEFORMAT, samplefmt);
        (void)TIFFSetField(tifw, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tifw, TIFFTAG_TILEWIDTH, tilew);
        (void)TIFFSetField(tifw, TIFFTAG_TILELENGTH, tileh);

        exercise_tiff(tifw, Data, Size);
        (void)TIFFCheckpointDirectory(tifw);
        exercise_tiff(tifw, Data, Size);

        TIFFClose(tifw);
    }

    return 0;
}