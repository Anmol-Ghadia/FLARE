// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFReverseBits at tif_swab.c:284:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFNumberOfTiles at tif_tile.c:113:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReverseBits at tif_swab.c:284:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
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

#include "tiffio.h"

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
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

int LLVMFuzzerTestOneInput_63(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    if (Size == 0) {
        TIFF *empty = TIFFOpen("./dummy_file", "r");
        if (empty) TIFFClose(empty);
        return 0;
    }

    size_t off = 0;

    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t fillorder = FILLORDER_MSB2LSB;
    uint32_t tilew = 16, tileh = 16;
    uint32_t rowsperstrip = 1;

    if (Size - off >= 4) { width = (rd32(Data + off) % 128) + 1; off += 4; }
    if (Size - off >= 4) { height = (rd32(Data + off) % 128) + 1; off += 4; }
    if (Size - off >= 2) { spp = (uint16_t)((rd16(Data + off) % 4) + 1); off += 2; }
    if (Size - off >= 2) {
        static const uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
        bps = bps_choices[rd16(Data + off) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
        off += 2;
    }
    if (Size - off >= 2) {
        static const uint16_t photo_choices[] = {
            PHOTOMETRIC_MINISBLACK, PHOTOMETRIC_MINISWHITE, PHOTOMETRIC_RGB
        };
        photo = photo_choices[rd16(Data + off) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
        off += 2;
        if (photo == PHOTOMETRIC_RGB && spp < 3) spp = 3;
    }
    if (Size - off >= 2) {
        static const uint16_t comp_choices[] = {
            COMPRESSION_NONE, COMPRESSION_LZW, COMPRESSION_PACKBITS, COMPRESSION_DEFLATE
        };
        compression = comp_choices[rd16(Data + off) % (sizeof(comp_choices) / sizeof(comp_choices[0]))];
        off += 2;
    }
    if (Size - off >= 2) { fillorder = (rd16(Data + off) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB; off += 2; }
    if (Size - off >= 4) { tilew = (rd32(Data + off) % 64) + 16; off += 4; }
    if (Size - off >= 4) { tileh = (rd32(Data + off) % 64) + 16; off += 4; }
    if (Size - off >= 4) { rowsperstrip = (rd32(Data + off) % height) + 1; off += 4; }

    if (tilew < 16) tilew = 16;
    if (tileh < 16) tileh = 16;

    TIFF *tw = TIFFOpen("./dummy_file", "w");
    if (!tw) return 0;

    TIFFSetField(tw, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tw, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tw, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tw, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tw, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tw, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tw, TIFFTAG_SAMPLEFORMAT, sampleformat);
    TIFFSetField(tw, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tw, TIFFTAG_FILLORDER, fillorder);

    int use_tiles = 0;
    if (off < Size) {
        use_tiles = Data[off] & 1;
        off++;
    }

    if (use_tiles) {
        TIFFSetField(tw, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tw, TIFFTAG_TILELENGTH, tileh);
    } else {
        TIFFSetField(tw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }

    tmsize_t unit_size = TIFFScanlineSize(tw);
    if (unit_size <= 0) unit_size = 1;

    tmsize_t write_size = use_tiles ? TIFFTileSize(tw) : TIFFStripSize(tw);
    if (write_size <= 0) write_size = unit_size;
    if (write_size <= 0 || write_size > (1 << 20)) write_size = (1 << 20);

    uint8_t *buf = (uint8_t *)malloc((size_t)write_size);
    if (!buf) {
        TIFFClose(tw);
        return 0;
    }

    if (Size - off > 0) {
        size_t remain = Size - off;
        size_t tocopy = remain < (size_t)write_size ? remain : (size_t)write_size;
        _TIFFmemcpy(buf, Data + off, (tmsize_t)tocopy);
        if ((size_t)write_size > tocopy) {
            size_t pos = tocopy;
            while (pos < (size_t)write_size) {
                size_t chunk = remain < ((size_t)write_size - pos) ? remain : ((size_t)write_size - pos);
                if (chunk == 0) break;
                _TIFFmemcpy(buf + pos, Data + off, (tmsize_t)chunk);
                pos += chunk;
            }
            while (pos < (size_t)write_size) {
                buf[pos++] = 0;
            }
        }
    } else {
        memset(buf, 0, (size_t)write_size);
    }

    TIFFReverseBits(buf, write_size > 64 ? 64 : write_size);

    if (use_tiles) {
        uint32_t num_tiles = TIFFNumberOfTiles(tw);
        if (num_tiles == 0) num_tiles = 1;
        uint32_t tile = 0;
        if (off < Size) tile = (uint32_t)(Data[off] % num_tiles);

        (void)TIFFWriteEncodedTile(tw, tile, buf, write_size);
        if (num_tiles > 1) {
            uint32_t tile2 = (tile + 1) % num_tiles;
            (void)TIFFWriteEncodedTile(tw, tile2, buf, write_size > 1 ? write_size - 1 : write_size);
        }
    } else {
        uint32_t num_strips = TIFFNumberOfStrips(tw);
        if (num_strips == 0) num_strips = 1;
        for (uint32_t s = 0; s < num_strips && s < 2; s++) {
            (void)TIFFWriteEncodedStrip(tw, s, buf, write_size);
        }
    }

    free(buf);
    TIFFClose(tw);

    TIFF *tr = TIFFOpen("./dummy_file", "r");
    if (!tr) return 0;

    tmsize_t rsize = use_tiles ? TIFFTileSize(tr) : TIFFStripSize(tr);
    if (rsize <= 0) rsize = TIFFScanlineSize(tr);
    if (rsize <= 0 || rsize > (1 << 20)) rsize = (1 << 20);

    uint8_t *rbuf = (uint8_t *)malloc((size_t)rsize);
    if (!rbuf) {
        TIFFClose(tr);
        return 0;
    }

    if (use_tiles) {
        uint32_t num_tiles = TIFFNumberOfTiles(tr);
        if (num_tiles == 0) num_tiles = 1;
        uint32_t tile = 0;
        if (Size > 0) tile = (uint32_t)(Data[0] % num_tiles);

        (void)TIFFReadEncodedTile(tr, tile, rbuf, rsize);
        (void)TIFFReadRawTile(tr, tile, rbuf, (off < Size && (Data[off] & 1)) ? (tmsize_t)-1 : rsize);
        if (num_tiles > 1) {
            uint32_t tile2 = (tile + 1) % num_tiles;
            (void)TIFFReadEncodedTile(tr, tile2, rbuf, rsize > 1 ? rsize - 1 : rsize);
            (void)TIFFReadRawTile(tr, tile2, rbuf, rsize);
        }
    } else {
        uint32_t num_strips = TIFFNumberOfStrips(tr);
        if (num_strips == 0) num_strips = 1;
        uint32_t strip = 0;
        if (Size > 0) strip = (uint32_t)(Data[0] % num_strips);

        (void)TIFFReadEncodedStrip(tr, strip, rbuf, rsize);
        if (num_strips > 1) {
            uint32_t strip2 = (strip + 1) % num_strips;
            (void)TIFFReadEncodedStrip(tr, strip2, rbuf, rsize > 1 ? rsize - 1 : rsize);
        }
    }

    TIFFReverseBits(rbuf, rsize > 64 ? 64 : rsize);

    free(rbuf);
    TIFFClose(tr);
    return 0;
}