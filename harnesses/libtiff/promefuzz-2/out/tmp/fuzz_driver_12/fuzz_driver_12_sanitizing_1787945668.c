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

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 1 >= n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 3 >= n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void set_common_tags(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0, 1);
    uint32_t length = rd32(Data, Size, 4, 1);
    uint16_t bps = (uint16_t)((rd16(Data, Size, 8, 8) % 32) + 1);
    uint16_t spp = (uint16_t)(rd16(Data, Size, 10, 1) % 8);
    uint16_t planar = (rd16(Data, Size, 12, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric_choices[] = {
        PHOTOMETRIC_MINISBLACK,
        PHOTOMETRIC_RGB,
        PHOTOMETRIC_PALETTE,
        PHOTOMETRIC_YCBCR
    };
    uint16_t photometric = photometric_choices[rd16(Data, Size, 14, 0) % 4];
    uint16_t compression = COMPRESSION_NONE;
    uint16_t fillorder = (rd16(Data, Size, 16, 0) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    uint16_t orientation = ORIENTATION_TOPLEFT;

    if (width == 0 && Size > 0) width = (uint32_t)Data[0];
    if (length == 0 && Size > 1) length = (uint32_t)Data[1];

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, length);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    if (photometric == PHOTOMETRIC_YCBCR) {
        uint16_t hsub = (uint16_t)((rd16(Data, Size, 18, 1) % 4) + 1);
        uint16_t vsub = (uint16_t)((rd16(Data, Size, 20, 1) % 4) + 1);
        TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, hsub, vsub);
    }

    if (tiled) {
        uint32_t tw = rd32(Data, Size, 24, 0);
        uint32_t tl = rd32(Data, Size, 28, 0);
        if ((rd16(Data, Size, 22, 0) & 1) == 0) {
            if (tw == 0) tw = 16;
            if (tl == 0) tl = 16;
        }
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tl);
    } else {
        uint32_t rps = rd32(Data, Size, 32, 0);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rps);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    set_common_tags(tif, Data, Size, 0);
    (void)TIFFScanlineSize64(tif);
    (void)TIFFStripSize64(tif);
    (void)TIFFVStripSize64(tif, 0);
    (void)TIFFVStripSize64(tif, 1);
    (void)TIFFVStripSize64(tif, (uint32_t)-1);
    (void)TIFFVStripSize64(tif, rd32(Data, Size, 36, 0));

    TIFFClose(tif);

    tif = TIFFOpen("./dummy_file", "w8");
    if (!tif) {
        return 0;
    }

    set_common_tags(tif, Data, Size, 1);
    (void)TIFFTileRowSize64(tif);
    (void)TIFFTileSize64(tif);
    (void)TIFFVTileSize64(tif, 0);
    (void)TIFFVTileSize64(tif, 1);
    (void)TIFFVTileSize64(tif, rd32(Data, Size, 40, 0));
    (void)TIFFScanlineSize64(tif);

    TIFFClose(tif);
    return 0;
}