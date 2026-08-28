#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1, rowsperstrip = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t fillorder = FILLORDER_MSB2LSB;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint32_t tilewidth = 16, tilelength = 16;
    int use_tiles = 0;

    if (Size >= 4) width = rd32(Data) % 2048 + 1;
    if (Size >= 8) height = rd32(Data + 4) % 2048 + 1;
    if (Size >= 10) spp = (uint16_t)(rd16(Data + 8) % 4 + 1);
    if (Size >= 12) {
        static const uint16_t bps_choices[] = {1, 2, 4, 8, 16, 32};
        bps = bps_choices[rd16(Data + 10) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    }
    if (Size >= 14) {
        static const uint16_t comp_choices[] = {
            COMPRESSION_NONE,
            COMPRESSION_LZW,
            COMPRESSION_PACKBITS,
            COMPRESSION_DEFLATE
        };
        compression = comp_choices[rd16(Data + 12) % (sizeof(comp_choices) / sizeof(comp_choices[0]))];
    }
    if (Size >= 16) {
        static const uint16_t photo_choices[] = {
            PHOTOMETRIC_MINISBLACK,
            PHOTOMETRIC_RGB,
            PHOTOMETRIC_PALETTE
        };
        photo = photo_choices[rd16(Data + 14) % (sizeof(photo_choices) / sizeof(photo_choices[0]))];
    }
    if (photo != PHOTOMETRIC_RGB) {
        spp = 1;
    } else if (spp < 3) {
        spp = 3;
    }

    if (Size >= 18) planar = (rd16(Data + 16) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size >= 20) sampleformat = (rd16(Data + 18) & 1) ? SAMPLEFORMAT_INT : SAMPLEFORMAT_UINT;
    if (Size >= 22) fillorder = (rd16(Data + 20) & 1) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    if (Size >= 24) orientation = (rd16(Data + 22) % 8) + 1;
    if (Size >= 28) rowsperstrip = rd32(Data + 24) % (height + 1);
    if (rowsperstrip == 0) rowsperstrip = 1;
    if (Size >= 29) use_tiles = Data[28] & 1;
    if (Size >= 33) tilewidth = rd32(Data + 29) % 512 + 1;
    if (Size >= 37) tilelength = rd32(Data + 33) % 512 + 1;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    if (use_tiles) {
        (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilewidth);
        (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, tilelength);
    } else {
        (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }

    tmsize_t scanline = TIFFScanlineSize(tif);
    tmsize_t rasterscanline = TIFFRasterScanlineSize(tif);
    tmsize_t stripsize = TIFFStripSize(tif);
    tmsize_t vstripsize1 = TIFFVStripSize(tif, 1);
    tmsize_t vstripsizeN = TIFFVStripSize(tif, rowsperstrip);

    (void)scanline;
    (void)rasterscanline;
    (void)stripsize;
    (void)vstripsize1;
    (void)vstripsizeN;

    if (use_tiles) {
        tmsize_t tilerow = TIFFTileRowSize(tif);
        (void)tilerow;
    }

    const uint8_t *payload = Data;
    size_t payload_size = Size;
    if (payload_size > 64) {
        payload += 37;
        payload_size -= 37;
    }

    if (payload_size == 0) {
        static const uint8_t zero = 0;
        payload = &zero;
        payload_size = 1;
    }

    uint32_t strip_indices[4];
    strip_indices[0] = 0;
    strip_indices[1] = 1;
    strip_indices[2] = (height > 0 && rowsperstrip > 0) ? (height / rowsperstrip) : 2;
    strip_indices[3] = 0xFFFFFFFFu;

    for (size_t i = 0; i < sizeof(strip_indices) / sizeof(strip_indices[0]); i++) {
        tmsize_t cc = (tmsize_t)(payload_size % 4096);
        if (i == 1 && scanline > 0) cc = scanline;
        if (i == 2 && stripsize > 0) cc = stripsize;
        if (i == 3) cc = (tmsize_t)payload_size;
        if (cc < 0) cc = (tmsize_t)payload_size;

        (void)TIFFWriteEncodedStrip(tif, strip_indices[i], (void *)payload, cc);
    }

    TIFFClose(tif);
    return 0;
}