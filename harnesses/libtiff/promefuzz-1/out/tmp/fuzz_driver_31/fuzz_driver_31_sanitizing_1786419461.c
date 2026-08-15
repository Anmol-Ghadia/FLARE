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

static uint32_t clamp_nonzero_u32(uint32_t v, uint32_t maxv) {
    if (maxv == 0) return 1;
    v %= maxv;
    if (v == 0) v = 1;
    return v;
}

static void setup_common_tags(TIFF *tif,
                              uint32_t width, uint32_t height,
                              uint16_t spp, uint16_t bps,
                              uint16_t planar, uint16_t photo,
                              uint16_t sampleformat,
                              uint32_t tilew, uint32_t tileh,
                              uint32_t rowsperstrip)
{
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    if (tilew != 0 && tileh != 0) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip ? rowsperstrip : 1);
    }
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

    uint32_t width = clamp_nonzero_u32(rd32(Data, Size, 0), 512);
    uint32_t height = clamp_nonzero_u32(rd32(Data, Size, 4), 512);
    uint32_t tilew = clamp_nonzero_u32(rd32(Data, Size, 8), 256);
    uint32_t tileh = clamp_nonzero_u32(rd32(Data, Size, 12), 256);
    uint32_t rowsperstrip = clamp_nonzero_u32(rd32(Data, Size, 16), 256);
    uint32_t x = rd32(Data, Size, 20);
    uint32_t y = rd32(Data, Size, 24);
    uint32_t z = rd32(Data, Size, 28);
    uint16_t s = rd16(Data, Size, 32);

    uint16_t spp = (uint16_t)((rd16(Data, Size, 34) % 4) + 1);
    uint16_t bps_choices[] = {1, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 36) % 4];
    uint16_t planar = (rd16(Data, Size, 38) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;

    if (planar == PLANARCONFIG_SEPARATE && spp != 0) {
        s %= spp;
    }

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        setup_common_tags(tifw, width, height, spp, bps, planar, photo,
                          sampleformat, tilew, tileh, 0);

        (void)TIFFNumberOfTiles(tifw);
        (void)TIFFComputeTile(tifw, x, y, z, s);
        (void)TIFFCheckTile(tifw, x, y, z, s);
        (void)TIFFVStripSize(tifw, rowsperstrip);

        tmsize_t tilesize = TIFFTileSize(tifw);
        if (tilesize > 0 && tilesize < (tmsize_t)(1 << 20)) {
            unsigned char *buf = (unsigned char *)_TIFFmalloc((tmsize_t)tilesize);
            if (buf) {
                size_t copy = Size < (size_t)tilesize ? Size : (size_t)tilesize;
                if (copy > 0) {
                    memcpy(buf, Data, copy);
                }
                if ((size_t)tilesize > copy) {
                    memset(buf + copy, 0, (size_t)tilesize - copy);
                }

                (void)TIFFWriteTile(tifw, buf, x, y, z, s);
                (void)TIFFWriteTile(tifw, buf, 0, 0, 0, 0);
                if (width > 0 && height > 0) {
                    uint32_t vx = (width > 1) ? (width - 1) : 0;
                    uint32_t vy = (height > 1) ? (height - 1) : 0;
                    uint16_t vs = (planar == PLANARCONFIG_SEPARATE && spp > 0) ? (uint16_t)(spp - 1) : 0;
                    (void)TIFFWriteTile(tifw, buf, vx, vy, 0, vs);
                }

                _TIFFfree(buf);
            }
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        (void)TIFFNumberOfTiles(tifr);
        (void)TIFFComputeTile(tifr, x, y, z, s);
        (void)TIFFCheckTile(tifr, x, y, z, s);
        (void)TIFFVStripSize(tifr, rowsperstrip);

        tmsize_t tilesize = TIFFTileSize(tifr);
        if (tilesize > 0 && tilesize < (tmsize_t)(1 << 20)) {
            unsigned char *buf = (unsigned char *)_TIFFmalloc((tmsize_t)tilesize);
            if (buf) {
                memset(buf, 0, (size_t)tilesize);
                (void)TIFFReadTile(tifr, buf, x, y, z, s);
                (void)TIFFReadTile(tifr, buf, 0, 0, 0, 0);
                if (width > 0 && height > 0) {
                    uint32_t vx = (width > 1) ? (width - 1) : 0;
                    uint32_t vy = (height > 1) ? (height - 1) : 0;
                    uint16_t vs = (planar == PLANARCONFIG_SEPARATE && spp > 0) ? (uint16_t)(spp - 1) : 0;
                    (void)TIFFReadTile(tifr, buf, vx, vy, 0, vs);
                }
                _TIFFfree(buf);
            }
        }

        TIFFClose(tifr);
    }

    return 0;
}