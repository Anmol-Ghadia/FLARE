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

static uint16_t rd16(const uint8_t *p, size_t n) {
    if (n < 2) return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n) {
    if (n < 4) {
        uint32_t v = 0;
        size_t i;
        for (i = 0; i < n; i++) v |= ((uint32_t)p[i]) << (8 * i);
        return v;
    }
    return (uint32_t)(p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
                      ((uint32_t)p[3] << 24));
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (!tifw) {
        tifw = TIFFOpen("./dummy_file", "w");
    }
    if (!tifw) {
        TIFF *tmp = TIFFOpen("./dummy_file", "r");
        if (tmp) TIFFClose(tmp);
        return 0;
    }

    uint32_t width = (Size > 0) ? ((uint32_t)Data[0] + 1U) : 1U;
    uint32_t height = (Size > 1) ? ((uint32_t)Data[1] + 1U) : 1U;
    uint16_t bps = (Size > 2) ? (uint16_t)((Data[2] % 8) + 1) : 8;
    uint16_t spp = (Size > 3) ? (uint16_t)((Data[3] % 4) + 1) : 1;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t orient = ORIENTATION_TOPLEFT;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t dirn = rd16(Data + (Size > 4 ? 4 : 0), Size > 4 ? Size - 4 : 0);
    uint64_t custom_off = 0;
    toff_t exif_off = (toff_t)rd32(Data + (Size > 8 ? 8 : 0), Size > 8 ? Size - 8 : 0);
    toff_t custom_read_off = (toff_t)rd32(Data + (Size > 12 ? 12 : 0), Size > 12 ? Size - 12 : 0);

    TIFFFreeDirectory(tifw);
    TIFFCreateCustomDirectory(tifw, NULL);
    TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
    TIFFWriteCustomDirectory(tifw, &custom_off);
    TIFFSetDirectory(tifw, dirn);
    TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFClose(tifw);

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (!tifr) {
        TIFF *tmp = TIFFOpen("./dummy_file", "r");
        if (tmp) TIFFClose(tmp);
        return 0;
    }

    uint32_t got_width = 0;
    uint32_t got_height = 0;
    uint16_t got_bps = 0;
    uint16_t got_spp = 0;
    uint16_t got_comp = 0;
    uint16_t got_photo = 0;
    uint16_t got_planar = 0;
    uint16_t got_orient = 0;
    uint16_t got_sampleformat = 0;

    TIFFGetField(tifr, TIFFTAG_IMAGEWIDTH, &got_width);
    TIFFGetField(tifr, TIFFTAG_IMAGELENGTH, &got_height);
    TIFFReadEXIFDirectory(tifr, exif_off ? exif_off : (toff_t)custom_off);
    TIFFGetField(tifr, TIFFTAG_BITSPERSAMPLE, &got_bps);
    TIFFReadCustomDirectory(tifr, custom_read_off ? custom_read_off : (toff_t)custom_off, NULL);
    TIFFGetField(tifr, TIFFTAG_COMPRESSION, &got_comp);
    TIFFGetField(tifr, TIFFTAG_PHOTOMETRIC, &got_photo);

    (void)photometric;
    (void)planar;
    (void)orient;
    (void)sampleformat;
    (void)got_spp;
    (void)got_planar;
    (void)got_orient;
    (void)got_sampleformat;

    TIFFClose(tifr);

    TIFF *tifx = TIFFOpen("./dummy_file", "r");
    if (tifx) {
        TIFFClose(tifx);
    }

    return 0;
}