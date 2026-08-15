// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
// TIFFGetStrileByteCount at tif_dirread.c:6405:10 in tiffio.h
// TIFFGetStrileOffset at tif_dirread.c:6390:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFRawStripSize64 at tif_strip.c:140:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFSetWriteOffset at tif_write.c:845:1 in tiffio.h
// TIFFWriteRawTile at tif_write.c:503:1 in tiffio.h
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
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    uint64_t v = 0;
    size_t i;
    for (i = 0; i < 8; i++) {
        if (off + i >= n) break;
        v |= ((uint64_t)p[off + i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    uint32_t width, height, tilew, tileh;
    uint16_t spp, bps, photometric, planarconfig, sampleformat, compression;
    uint32_t tile_count = 0;
    uint32_t i, idx1, idx2;
    uint64_t offv;
    int err = 0;
    tmsize_t write_len;
    size_t payload_off;
    size_t payload_size;

    fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return 0;

    width = (uint32_t)(1 + (rd32(Data, Size, 0) % 128));
    height = (uint32_t)(1 + (rd32(Data, Size, 4) % 128));
    tilew = (uint32_t)(16u << (rd16(Data, Size, 8) % 3));
    tileh = (uint32_t)(16u << (rd16(Data, Size, 10) % 3));
    if (tilew > width) tilew = width;
    if (tileh > height) tileh = height;
    if (tilew == 0) tilew = 16;
    if (tileh == 0) tileh = 16;

    spp = (uint16_t)(1 + (rd16(Data, Size, 12) % 4));
    bps = (rd16(Data, Size, 14) & 1) ? 16 : 8;
    photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    planarconfig = (rd16(Data, Size, 16) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    sampleformat = (rd16(Data, Size, 18) & 1) ? SAMPLEFORMAT_UINT : SAMPLEFORMAT_INT;
    compression = COMPRESSION_NONE;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);

    tile_count = TIFFNumberOfTiles(tif);

    offv = rd64(Data, Size, 24);
    TIFFSetWriteOffset(tif, (toff_t)offv);

    idx1 = tile_count ? (rd32(Data, Size, 32) % tile_count) : 0;
    idx2 = rd32(Data, Size, 36);

    payload_off = 44;
    payload_size = (Size > payload_off) ? (Size - payload_off) : 0;
    write_len = (tmsize_t)payload_size;

    if (payload_size > 0 && tile_count > 0) {
        (void)TIFFWriteRawTile(tif, idx1, (void *)(Data + payload_off), write_len);

        if (payload_size > 1) {
            tmsize_t half = (tmsize_t)(payload_size / 2);
            (void)TIFFWriteRawTile(tif, idx1, (void *)(Data + payload_off), half);
        }

        (void)TIFFWriteRawTile(tif, idx2, (void *)(Data + payload_off), write_len);
    } else if (tile_count > 0) {
        uint8_t dummy = 0;
        (void)TIFFWriteRawTile(tif, idx1, &dummy, 0);
        (void)TIFFWriteRawTile(tif, idx2, &dummy, 0);
    }

    for (i = 0; i < 4; i++) {
        uint32_t sidx = (i == 0) ? 0 :
                        (i == 1) ? idx1 :
                        (i == 2) ? idx2 :
                        (tile_count ? (tile_count - 1) : 0);
        (void)TIFFGetStrileByteCount(tif, sidx);
        (void)TIFFGetStrileOffset(tif, sidx);
        err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, sidx, &err);
        (void)TIFFRawStripSize64(tif, sidx);
    }

    TIFFClose(tif);
    return 0;
}