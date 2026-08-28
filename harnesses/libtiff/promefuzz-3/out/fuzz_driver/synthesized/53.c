// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteEncodedTile at tif_write.c:391:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteEncodedStrip at tif_write.c:212:1 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFGetStrileByteCountWithErr at tif_dirread.c:6411:10 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFPrintDirectory at tif_print.c:235:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    uint16_t compression_choices[] = {COMPRESSION_NONE, COMPRESSION_LZW, COMPRESSION_PACKBITS, COMPRESSION_ADOBE_DEFLATE};
    uint16_t compression = compression_choices[rd16(Data, Size, 12, 0) % 4];

    if (width == 0) width = 1;
    if (height == 0) height = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    if (compression == COMPRESSION_LZW || compression == COMPRESSION_ADOBE_DEFLATE) {
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
    if (Size > 24) flags = (long)(rd32(Data, Size, 24, 0) & 0xFFFF);
    FILE *fp = fopen("/dev/null", "wb");
    if (fp) {
        TIFFPrintDirectory(tif, fp, flags);
        fclose(fp);
    }
}

static void *make_sized_copy(const uint8_t *src, size_t src_size, size_t need) {
    uint8_t *buf;
    size_t i;

    if (need == 0) return NULL;
    buf = (uint8_t *)malloc(need);
    if (!buf) return NULL;
    if (src_size == 0) {
        memset(buf, 0, need);
        return buf;
    }
    for (i = 0; i < need; ++i) {
        buf[i] = src[i % src_size];
    }
    return buf;
}

int LLVMFuzzerTestOneInput_53(const uint8_t *Data, size_t Size) {
    FILE *out = fopen("./dummy_file", "wb");
    if (!out) return 0;
    if (Size > 0) fwrite(Data, 1, Size, out);
    fclose(out);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        remove("./dummy_file");
        return 0;
    }

    {
        int tiled = (Size > 0 && (Data[0] & 1)) ? 1 : 0;
        const uint8_t *payload = (Size > 32) ? Data + 32 : Data;
        size_t payload_size = (Size > 32) ? (Size - 32) : Size;

        set_common_fields(tif, Data, Size, tiled);

        if (tiled) {
            tmsize_t tile_size = TIFFTileSize(tif);
            tmsize_t scanline = TIFFScanlineSize(tif);
            uint32_t tile = rd32(Data, Size, 28, 0);
            tmsize_t ccs[3];
            int i;

            if (tile_size < 0) tile_size = 0;
            if (scanline < 0) scanline = 0;

            ccs[0] = tile_size;
            ccs[1] = (scanline > 0) ? scanline : (tile_size > 1 ? tile_size / 2 : tile_size);
            ccs[2] = (payload_size > 0) ? (tmsize_t)payload_size : tile_size;

            for (i = 0; i < 3; ++i) {
                uint32_t cur_tile = (i == 0) ? tile : (i == 1 ? (tile ^ 1U) : (tile + 1U));
                tmsize_t cc = ccs[i];
                void *buf = NULL;

                if (cc > 0) {
                    buf = make_sized_copy(payload, payload_size, (size_t)cc);
                    if (buf) {
                        (void)TIFFWriteEncodedTile(tif, cur_tile, buf, cc);
                        free(buf);
                    }
                }
            }

            {
                int err = 0;
                (void)TIFFGetStrileByteCountWithErr(tif, tile, &err);
                err = 0;
                (void)TIFFGetStrileByteCountWithErr(tif, tile + 1U, &err);
            }
        } else {
            tmsize_t scanline = TIFFScanlineSize(tif);
            uint32_t strip = rd32(Data, Size, 28, 0);
            tmsize_t ccs[3];
            int i;

            if (scanline < 0) scanline = 0;

            ccs[0] = scanline;
            ccs[1] = (payload_size > 0) ? (tmsize_t)payload_size : scanline;
            ccs[2] = (scanline > 1) ? (scanline - 1) : scanline;

            for (i = 0; i < 3; ++i) {
                uint32_t cur_strip = (i == 0) ? strip : (i == 1 ? (strip + 1U) : (strip ^ 1U));
                tmsize_t cc = ccs[i];
                void *buf = NULL;

                if (cc > 0) {
                    buf = make_sized_copy(payload, payload_size, (size_t)cc);
                    if (buf) {
                        (void)TIFFWriteEncodedStrip(tif, cur_strip, buf, cc);
                        free(buf);
                    }
                }
            }

            {
                int err = 0;
                (void)TIFFGetStrileByteCountWithErr(tif, strip, &err);
                err = 0;
                (void)TIFFGetStrileByteCountWithErr(tif, strip + 1U, &err);
            }
        }

        fuzz_print_dir(tif, Data, Size);
    }

    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}