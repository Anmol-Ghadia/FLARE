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
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFTileRowSize at tif_tile.c:180:1 in tiffio.h
// TIFFVTileSize at tif_tile.c:240:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadEncodedTile at tif_read.c:913:1 in tiffio.h
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t size, size_t off, uint16_t defv) {
    if (off + 1 >= size) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t size, size_t off, uint32_t defv) {
    if (off + 3 >= size) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static tmsize_t clamp_tmsize_u64(uint64_t v) {
    if (v > (uint64_t)((tmsize_t)-1 > 0 ? (tmsize_t)-1 : ((((uint64_t)1) << (sizeof(tmsize_t) * 8 - 1)) - 1)))
        return (tmsize_t)1024;
    return (tmsize_t)v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        do {
            (void)TIFFTileSize(tif);
            (void)TIFFTileRowSize(tif);

            uint32_t nrows = rd32(Data, Size, 0, 1);
            (void)TIFFVTileSize(tif, nrows);

            uint32_t tile = rd32(Data, Size, 4, 0);
            tmsize_t tile_size = TIFFTileSize(tif);
            tmsize_t row_size = TIFFTileRowSize(tif);
            tmsize_t vtile_size = TIFFVTileSize(tif, nrows);

            size_t alloc_sz = 4096;
            if (tile_size > 0 && (size_t)tile_size < alloc_sz) alloc_sz = (size_t)tile_size;
            if (row_size > 0 && (size_t)row_size > alloc_sz && (size_t)row_size < (1U << 20)) alloc_sz = (size_t)row_size;
            if (vtile_size > 0 && (size_t)vtile_size > alloc_sz && (size_t)vtile_size < (1U << 20)) alloc_sz = (size_t)vtile_size;
            if (alloc_sz == 0) alloc_sz = 1;

            void *buf = malloc(alloc_sz);
            if (buf) {
                memset(buf, 0, alloc_sz);

                (void)TIFFReadRawTile(tif, tile, buf, (tmsize_t)alloc_sz);
                (void)TIFFReadRawTile(tif, tile, buf, (tmsize_t)-1);

                (void)TIFFReadEncodedTile(tif, tile, buf, (tmsize_t)alloc_sz);
                (void)TIFFReadEncodedTile(tif, tile, buf, (tmsize_t)-1);

                if (alloc_sz > 1) {
                    (void)TIFFReadRawTile(tif, tile ^ 1U, buf, (tmsize_t)(alloc_sz / 2));
                    (void)TIFFReadEncodedTile(tif, tile + 1U, buf, (tmsize_t)(alloc_sz / 2));
                }

                free(buf);
            }
        } while (TIFFReadDirectory(tif));
        TIFFClose(tif);
    }

    TIFF *wtif = TIFFOpen("./dummy_file", "w");
    if (wtif) {
        uint32_t width = rd32(Data, Size, 8, 16) % 512 + 1;
        uint32_t height = rd32(Data, Size, 12, 16) % 512 + 1;
        uint16_t spp = (uint16_t)((rd16(Data, Size, 16, 1) % 4) + 1);
        uint16_t bps_choices[] = {1, 8, 16, 32};
        uint16_t bps = bps_choices[rd16(Data, Size, 18, 1) % 4];
        uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        uint16_t planar = PLANARCONFIG_CONTIG;
        uint16_t compression = COMPRESSION_NONE;
        uint32_t tilew = (rd32(Data, Size, 20, 16) % 128) + 1;
        uint32_t tileh = (rd32(Data, Size, 24, 16) % 128) + 1;

        if (tilew < 16) tilew = 16;
        if (tileh < 16) tileh = 16;

        (void)TIFFSetField(wtif, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(wtif, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(wtif, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(wtif, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(wtif, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(wtif, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(wtif, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(wtif, TIFFTAG_TILEWIDTH, tilew);
        (void)TIFFSetField(wtif, TIFFTAG_TILELENGTH, tileh);

        (void)TIFFTileSize(wtif);
        (void)TIFFTileRowSize(wtif);
        (void)TIFFVTileSize(wtif, rd32(Data, Size, 28, tileh));

        tmsize_t ts = TIFFTileSize(wtif);
        if (ts <= 0 || ts > (1 << 20)) {
            ts = 4096;
        }

        void *wbuf = malloc((size_t)ts);
        if (wbuf) {
            if (Size > 0) {
                size_t copy = (Size < (size_t)ts) ? Size : (size_t)ts;
                memcpy(wbuf, Data, copy);
                if (copy < (size_t)ts) {
                    memset((uint8_t *)wbuf + copy, 0, (size_t)ts - copy);
                }
            } else {
                memset(wbuf, 0, (size_t)ts);
            }

            uint32_t wt = rd32(Data, Size, 32, 0);
            (void)TIFFWriteEncodedTile(wtif, wt, wbuf, ts);
            (void)TIFFWriteEncodedTile(wtif, wt ^ 1U, wbuf, ts / 2);
            (void)TIFFWriteEncodedTile(wtif, wt + 1U, wbuf, clamp_tmsize_u64((uint64_t)(size_t)ts));

            free(wbuf);
        }

        TIFFClose(wtif);
    }

    return 0;
}