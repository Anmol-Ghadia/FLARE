#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 2 > n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 4 > n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static void set_common_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd16(Data, Size, 0, 16);
    uint32_t height = rd16(Data, Size, 2, 16);
    uint16_t spp = (uint16_t)((rd16(Data, Size, 4, 1) % 4) + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16};
    uint16_t bps = bps_choices[rd16(Data, Size, 6, 0) % 5];
    uint16_t photo = (uint16_t)(rd16(Data, Size, 8, 0) % 6);
    uint16_t planar = (rd16(Data, Size, 10, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression_choices[] = {COMPRESSION_NONE, COMPRESSION_LZW, COMPRESSION_PACKBITS, COMPRESSION_DEFLATE};
    uint16_t compression = compression_choices[rd16(Data, Size, 12, 0) % 4];
    uint16_t orientation = ORIENTATION_TOPLEFT;

    if (width == 0) width = 1;
    if (height == 0) height = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    if (compression == COMPRESSION_LZW || compression == COMPRESSION_DEFLATE) {
        TIFFSetField(tif, TIFFTAG_PREDICTOR, (rd16(Data, Size, 14, 0) & 1) ? 2 : 1);
    }

    if (tiled) {
        uint32_t tw = rd16(Data, Size, 16, 16);
        uint32_t tl = rd16(Data, Size, 18, 16);
        if (tw < 16) tw = 16;
        if (tl < 16) tl = 16;
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tl);
    } else {
        uint32_t rowsperstrip = rd16(Data, Size, 20, 1);
        if (rowsperstrip == 0) rowsperstrip = 1;
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

static void fuzz_print_dir(TIFF *tif, const uint8_t *Data, size_t Size) {
    long flags = 0;
    if (Size > 24) {
        flags = (long)(rd32(Data, Size, 24, 0) & 0xFFFF);
    }
    FILE *fp = fopen("/dev/null", "wb");
    if (fp) {
        TIFFPrintDirectory(tif, fp, flags);
        fclose(fp);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *out = fopen("./dummy_file", "wb");
    if (!out) return 0;
    if (Size > 0) fwrite(Data, 1, Size, out);
    fclose(out);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        remove("./dummy_file");
        return 0;
    }

    int tiled = (Size > 0 && (Data[0] & 1)) ? 1 : 0;
    set_common_fields(tif, Data, Size, tiled);

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline < 0) scanline = 0;

    if (tiled) {
        tmsize_t tile_size = TIFFTileSize(tif);
        if (tile_size < 0) tile_size = 0;

        uint32_t tile = rd32(Data, Size, 28, 0);
        tmsize_t cc1 = tile_size;
        tmsize_t cc2 = (scanline > 0) ? scanline : tile_size / 2;
        tmsize_t cc3 = (Size > 32) ? (tmsize_t)(Size - 32) : 0;

        const uint8_t *buf = (Size > 32) ? Data + 32 : Data;
        if (cc1 > 0) (void)TIFFWriteEncodedTile(tif, tile, (void *)buf, cc1);
        if (cc2 > 0) (void)TIFFWriteEncodedTile(tif, tile ^ 1U, (void *)buf, cc2);
        if (cc3 > 0) (void)TIFFWriteEncodedTile(tif, tile + 1U, (void *)buf, cc3);

        int err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, tile, &err);
        err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, tile + 1U, &err);
    } else {
        uint32_t strip = rd32(Data, Size, 28, 0);
        tmsize_t cc1 = scanline;
        tmsize_t cc2 = (Size > 32) ? (tmsize_t)(Size - 32) : 0;
        tmsize_t cc3 = (scanline > 1) ? (scanline - 1) : scanline;

        const uint8_t *buf = (Size > 32) ? Data + 32 : Data;
        if (cc1 > 0) (void)TIFFWriteEncodedStrip(tif, strip, (void *)buf, cc1);
        if (cc2 > 0) (void)TIFFWriteEncodedStrip(tif, strip + 1U, (void *)buf, cc2);
        if (cc3 > 0) (void)TIFFWriteEncodedStrip(tif, strip ^ 1U, (void *)buf, cc3);

        int err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, strip, &err);
        err = 0;
        (void)TIFFGetStrileByteCountWithErr(tif, strip + 1U, &err);
    }

    fuzz_print_dir(tif, Data, Size);
    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}