#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "tiffio.h"

static uint16_t rd16(const uint8_t *p, size_t n) {
    if (n < 2) return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n) {
    uint32_t v = 0;
    size_t i, lim = n < 4 ? n : 4;
    for (i = 0; i < lim; i++) {
        v |= ((uint32_t)p[i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tifw = NULL;
    TIFF *tifr = NULL;
    TIFF *tifx = NULL;
    uint64_t custom_off = 0;
    uint16_t dirn;
    toff_t exif_off;
    toff_t custom_read_off;
    uint32_t width, height;
    uint16_t bps, spp;
    uint32_t got_width = 0, got_height = 0;
    uint16_t got_bps = 0, got_comp = 0, got_photo = 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    tifw = TIFFOpen("./dummy_file", "w8");
    if (!tifw) tifw = TIFFOpen("./dummy_file", "w");
    if (!tifw) {
        tifx = TIFFOpen("./dummy_file", "r");
        if (tifx) TIFFClose(tifx);
        return 0;
    }

    width = (Size > 0) ? ((uint32_t)Data[0] + 1U) : 1U;
    height = (Size > 1) ? ((uint32_t)Data[1] + 1U) : 1U;
    bps = (Size > 2) ? (uint16_t)((Data[2] % 8) + 1) : 8;
    spp = (Size > 3) ? (uint16_t)((Data[3] % 4) + 1) : 1;
    dirn = rd16(Data + (Size > 4 ? 4 : 0), Size > 4 ? Size - 4 : 0);
    exif_off = (toff_t)rd32(Data + (Size > 8 ? 8 : 0), Size > 8 ? Size - 8 : 0);
    custom_read_off = (toff_t)rd32(Data + (Size > 12 ? 12 : 0), Size > 12 ? Size - 12 : 0);

    TIFFFreeDirectory(tifw);
    TIFFCreateDirectory(tifw);
    TIFFSetField(tifw, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tifw, TIFFTAG_IMAGELENGTH, height);
    TIFFWriteDirectory(tifw);
    TIFFSetDirectory(tifw, dirn);
    TIFFSetField(tifw, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tifw, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFClose(tifw);

    tifr = TIFFOpen("./dummy_file", "r");
    if (!tifr) {
        tifx = TIFFOpen("./dummy_file", "r");
        if (tifx) TIFFClose(tifx);
        return 0;
    }

    TIFFGetField(tifr, TIFFTAG_IMAGEWIDTH, &got_width);
    TIFFGetField(tifr, TIFFTAG_IMAGELENGTH, &got_height);
    TIFFReadEXIFDirectory(tifr, exif_off ? exif_off : (toff_t)custom_off);
    TIFFGetField(tifr, TIFFTAG_BITSPERSAMPLE, &got_bps);
    if (custom_off != 0) {
        TIFFReadCustomDirectory(tifr, custom_read_off ? custom_read_off : (toff_t)custom_off, NULL);
    }
    TIFFGetField(tifr, TIFFTAG_COMPRESSION, &got_comp);
    TIFFGetField(tifr, TIFFTAG_PHOTOMETRIC, &got_photo);
    TIFFClose(tifr);

    tifx = TIFFOpen("./dummy_file", "r");
    if (tifx) TIFFClose(tifx);

    (void)got_width;
    (void)got_height;
    (void)got_bps;
    (void)got_comp;
    (void)got_photo;

    return 0;
}