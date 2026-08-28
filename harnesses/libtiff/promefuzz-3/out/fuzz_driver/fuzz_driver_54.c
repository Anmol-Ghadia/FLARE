// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFWriteTile at tif_write.c:364:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFReadTile at tif_read.c:900:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
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
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    uint32_t width = 32, height = 32;
    uint32_t tilew = 16, tileh = 16;
    uint16_t spp = 1, bps = 8;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;

    if (Size >= 2) {
        width = (rd16(Data) % 128) + 1;
    }
    if (Size >= 4) {
        height = (rd16(Data + 2) % 128) + 1;
    }
    if (Size >= 6) {
        tilew = ((rd16(Data + 4) % 32) + 1);
    }
    if (Size >= 8) {
        tileh = ((rd16(Data + 6) % 32) + 1);
    }
    if (Size >= 9) {
        spp = (uint16_t)((Data[8] % 4) + 1);
    }
    if (Size >= 10) {
        bps = (Data[9] & 1) ? 8 : 16;
    }
    if (Size >= 11) {
        compression = (Data[10] & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size >= 12) {
        planar = (Data[11] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size >= 13) {
        photometric = (Data[12] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    }

    if (photometric == PHOTOMETRIC_RGB && spp < 3)
        spp = 3;

    if (tilew > width) tilew = width;
    if (tileh > height) tileh = height;

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (!tifw) {
        return 0;
    }

    TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tifw, TIFFTAG_SAMPLEFORMAT, sampleformat);
    TIFFSetField(tifw, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tifw, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tifw, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tifw, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tifw, TIFFTAG_TILEWIDTH, tilew);
    TIFFSetField(tifw, TIFFTAG_TILELENGTH, tileh);

    tmsize_t tile_size = TIFFVTileSize(tifw, tileh);
    if (tile_size <= 0 || tile_size > (tmsize_t)(1 << 20)) {
        TIFFClose(tifw);
        return 0;
    }

    void *tilebuf = _TIFFmalloc((tmsize_t)tile_size);
    if (!tilebuf) {
        TIFFClose(tifw);
        return 0;
    }
    memset(tilebuf, 0, (size_t)tile_size);

    if (Size > 0) {
        size_t copy = Size < (size_t)tile_size ? Size : (size_t)tile_size;
        memcpy(tilebuf, Data, copy);
    }

    uint32_t x = 0, y = 0, z = 0;
    uint16_t s = 0;
    if (Size >= 17) {
        x = width ? (rd32(Data + 13) % width) : 0;
    }
    if (Size >= 21) {
        y = height ? (rd32(Data + 17) % height) : 0;
    }
    if (Size >= 23) {
        s = (uint16_t)(rd16(Data + 21) % spp);
    }

    (void)TIFFWriteTile(tifw, tilebuf, x, y, z, s);

    ttile_t num_tiles = TIFFNumberOfTiles(tifw);
    if (num_tiles > 0) {
        uint32_t tile_index = 0;
        if (Size >= 27) {
            tile_index = rd32(Data + 23) % (uint32_t)num_tiles;
        }
        (void)TIFFWriteEncodedTile(tifw, tile_index, tilebuf, tile_size);
        if (Size >= 28) {
            tmsize_t partial = (tmsize_t)(Data[27] % ((tile_size > 0) ? tile_size : 1));
            (void)TIFFWriteEncodedTile(tifw, tile_index, tilebuf, partial);
        }
    }

    TIFFClose(tifw);
    _TIFFfree(tilebuf);

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (!tifr) {
        return 0;
    }

    uint32_t rtilew = 0, rtileh = 0;
    if (!TIFFGetField(tifr, TIFFTAG_TILEWIDTH, &rtilew) ||
        !TIFFGetField(tifr, TIFFTAG_TILELENGTH, &rtileh) ||
        rtilew == 0 || rtileh == 0) {
        TIFFClose(tifr);
        return 0;
    }

    tmsize_t read_tile_size = TIFFVTileSize(tifr, rtileh);
    if (read_tile_size <= 0 || read_tile_size > (tmsize_t)(1 << 20)) {
        TIFFClose(tifr);
        return 0;
    }

    void *readbuf = _TIFFmalloc((tmsize_t)read_tile_size);
    if (!readbuf) {
        TIFFClose(tifr);
        return 0;
    }
    memset(readbuf, 0, (size_t)read_tile_size);

    uint32_t rx = 0, ry = 0;
    uint16_t rs = 0;
    if (Size >= 32) {
        uint32_t imgw = 0, imgh = 0;
        TIFFGetField(tifr, TIFFTAG_IMAGEWIDTH, &imgw);
        TIFFGetField(tifr, TIFFTAG_IMAGELENGTH, &imgh);
        rx = imgw ? (rd32(Data + 28) % imgw) : 0;
        ry = imgh ? (rd32(Data + 24) % imgh) : 0;
    }
    if (Size >= 33) {
        uint16_t rspp = 1;
        TIFFGetField(tifr, TIFFTAG_SAMPLESPERPIXEL, &rspp);
        if (rspp == 0) rspp = 1;
        rs = (uint16_t)(Data[32] % rspp);
    }

    (void)TIFFReadTile(tifr, readbuf, rx, ry, 0, rs);

    ttile_t rnum_tiles = TIFFNumberOfTiles(tifr);
    if (rnum_tiles > 0) {
        uint32_t rtile = 0;
        if (Size >= 37) {
            rtile = rd32(Data + 33) % (uint32_t)rnum_tiles;
        }

        (void)TIFFReadEncodedTile(tifr, rtile, readbuf, read_tile_size);
        (void)TIFFReadEncodedTile(tifr, rtile, readbuf, (read_tile_size > 1) ? (read_tile_size / 2) : read_tile_size);
        (void)TIFFReadRawTile(tifr, rtile, readbuf, (tmsize_t)-1);
        (void)TIFFReadRawTile(tifr, rtile, readbuf, read_tile_size);
    }

    _TIFFfree(readbuf);
    TIFFClose(tifr);
    return 0;
}