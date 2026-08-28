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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const char *path = "./dummy_file";
    FILE *fp = fopen(path, "wb");
    if (!fp) return 0;
    if (Size > 0) (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen(path, "w+");
    TIFF *tif_read_tile = NULL;
    TIFF *tif_read_strip = NULL;

    if (!tif) {
        tif_read_tile = TIFFOpen(path, "r");
        if (tif_read_tile) {
            uint8_t buf[16];
            (void)TIFFReadEncodedTile(tif_read_tile, 0, buf, (tmsize_t)sizeof(buf));
            TIFFClose(tif_read_tile);
        }
        tif_read_strip = TIFFOpen(path, "r");
        if (tif_read_strip) {
            uint8_t buf[16];
            (void)TIFFReadEncodedStrip(tif_read_strip, 0, buf, (tmsize_t)sizeof(buf));
            TIFFClose(tif_read_strip);
        }
        return 0;
    }

    uint32_t width = rd32(Data, Size, 0, 16) % 64 + 1;
    uint32_t height = rd32(Data, Size, 4, 16) % 64 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 1) % 4];
    uint16_t photo = (uint16_t)((rd16(Data, Size, 12, 1) % 2) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
    if (spp < 3 && photo == PHOTOMETRIC_RGB) photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (rd16(Data, Size, 14, 1) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t tilew = rd32(Data, Size, 16, 16) % 64 + 1;
    uint32_t tileh = rd32(Data, Size, 20, 16) % 64 + 1;
    uint32_t rowsperstrip = rd32(Data, Size, 24, 16) % 64 + 1;

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    if (Data[0] & 1) {
        if (tilew < 16) tilew = 16;
        if (tileh < 16) tileh = 16;
        (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
    } else {
        (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }

    (void)TIFFDeferStrileArrayWriting(tif);
    (void)TIFFWriteCheck(tif, (Data[0] & 1) ? 1 : 0, "fuzz");
    (void)TIFFWriteDirectory(tif);

    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFForceStrileArrayWriting(tif);
    (void)TIFFSetDirectory(tif, 1);
    (void)TIFFForceStrileArrayWriting(tif);
    (void)TIFFSetDirectory(tif, (uint16_t)(rd16(Data, Size, 28, 0) % 4));

    tmsize_t payload_len = (tmsize_t)(Size > 32 ? (Size - 32) : Size);
    void *payload = (void *)(uintptr_t)(Data + ((Size > 32) ? 32 : 0));
    if (payload_len <= 0) {
        payload = (void *)(uintptr_t)Data;
        payload_len = (tmsize_t)Size;
    }

    (void)TIFFWriteEncodedTile(tif, 0, payload, payload_len);
    (void)TIFFWriteEncodedStrip(tif, 0, payload, payload_len);
    TIFFClose(tif);

    tif_read_tile = TIFFOpen(path, "r");
    if (tif_read_tile) {
        uint8_t *rbuf = NULL;
        tmsize_t rsize = 4096;
        rbuf = (uint8_t *)malloc((size_t)rsize);
        if (rbuf) {
            (void)TIFFReadEncodedTile(tif_read_tile, 0, rbuf, rsize);
            free(rbuf);
        } else {
            uint8_t smallbuf[16];
            (void)TIFFReadEncodedTile(tif_read_tile, 0, smallbuf, (tmsize_t)sizeof(smallbuf));
        }
        TIFFClose(tif_read_tile);
    }

    tif_read_strip = TIFFOpen(path, "r");
    if (tif_read_strip) {
        uint8_t *rbuf = NULL;
        tmsize_t rsize = 4096;
        rbuf = (uint8_t *)malloc((size_t)rsize);
        if (rbuf) {
            (void)TIFFReadEncodedStrip(tif_read_strip, 0, rbuf, rsize);
            free(rbuf);
        } else {
            uint8_t smallbuf[16];
            (void)TIFFReadEncodedStrip(tif_read_strip, 0, smallbuf, (tmsize_t)sizeof(smallbuf));
        }
        TIFFClose(tif_read_strip);
    }

    return 0;
}