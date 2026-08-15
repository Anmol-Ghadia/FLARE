#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *f = fopen("./dummy_file", "wb");
    if (!f) return;
    if (Size > 0) fwrite(Data, 1, Size, f);
    fclose(f);
}

static void setup_basic_fields(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = rd32(Data, Size, 0);
    uint32_t height = rd32(Data, Size, 4);
    uint16_t spp = (uint16_t)((rd16(Data, Size, 8) % 4) + 1);
    uint16_t bps = (uint16_t)(1U << (rd16(Data, Size, 10) % 4));
    uint16_t photometric = (uint16_t)(rd16(Data, Size, 12) % 6);
    uint16_t compression = (rd16(Data, Size, 14) & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    uint16_t planar = (rd16(Data, Size, 16) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t orientation = (uint16_t)((rd16(Data, Size, 18) % 8) + 1);

    if (width == 0) width = 1;
    if (height == 0) height = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    switch (photometric) {
        case 0: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE); break;
        case 1: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK); break;
        case 2: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); break;
        case 3: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE); break;
        case 4: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MASK); break;
        default: TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK); break;
    }

    if (tiled) {
        uint32_t tw = rd32(Data, Size, 20);
        uint32_t th = rd32(Data, Size, 24);
        if (tw == 0) tw = 16;
        if (th == 0) th = 16;
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    } else {
        uint32_t rowsperstrip = rd32(Data, Size, 28);
        if (rowsperstrip == 0) rowsperstrip = 1;
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif) {
            uint8_t *buf1 = NULL;
            uint8_t *buf2 = NULL;
            tmsize_t s1 = (Size > 0) ? (tmsize_t)(Data[0] % 4096) : 0;
            tmsize_t s2 = (Size > 1) ? (tmsize_t)(Data[1] % 4096) : 1;

            if (s2 <= 0) s2 = 1;
            buf2 = (uint8_t *)malloc((size_t)s2);
            if (buf2) memset(buf2, 0, (size_t)s2);

            TIFFReadBufferSetup(tif, NULL, s1);
            TIFFReadBufferSetup(tif, buf2, s2);
            TIFFIsTiled(tif);
            TIFFFlush(tif);
            TIFFClose(tif);
            free(buf2);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w");
        if (tif) {
            int tiled = (Size > 2) ? (Data[2] & 1) : 0;
            uint8_t *wbuf = NULL;
            tmsize_t wsize = (Size > 3) ? (tmsize_t)(rd16(Data, Size, 30) % 8192) : (tmsize_t)-1;
            tmsize_t rsize = (Size > 4) ? (tmsize_t)(rd16(Data, Size, 32) % 8192) : 128;

            if (rsize <= 0) rsize = 128;
            setup_basic_fields(tif, Data, Size, tiled);

            if (wsize > 0) {
                wbuf = (uint8_t *)malloc((size_t)wsize);
                if (wbuf) memset(wbuf, 0xA5, (size_t)wsize);
            }

            TIFFWriteBufferSetup(tif, wbuf, wbuf ? wsize : (tmsize_t)-1);
            TIFFReadBufferSetup(tif, NULL, rsize);
            TIFFIsTiled(tif);
            TIFFSetupStrips(tif);
            TIFFFlush(tif);
            TIFFForceStrileArrayWriting(tif);
            TIFFFlush(tif);
            TIFFClose(tif);
            free(wbuf);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w+");
        if (tif) {
            int tiled = (Size > 5) ? (Data[5] & 1) : 1;
            uint8_t *bufa = NULL;
            uint8_t *bufb = NULL;
            tmsize_t sa = (Size > 6) ? (tmsize_t)(rd16(Data, Size, 34) % 16384) : 256;
            tmsize_t sb = (Size > 7) ? (tmsize_t)(rd16(Data, Size, 36) % 16384) : (tmsize_t)-1;

            if (sa <= 0) sa = 256;
            bufa = (uint8_t *)malloc((size_t)sa);
            if (bufa) memset(bufa, 0x5A, (size_t)sa);

            if (sb > 0) {
                bufb = (uint8_t *)malloc((size_t)sb);
                if (bufb) memset(bufb, 0, (size_t)sb);
            }

            setup_basic_fields(tif, Data, Size, tiled);

            TIFFWriteBufferSetup(tif, bufa, sa);
            TIFFWriteBufferSetup(tif, NULL, (tmsize_t)-1);
            TIFFReadBufferSetup(tif, bufb, bufb ? sb : 64);
            TIFFIsTiled(tif);
            TIFFSetupStrips(tif);
            TIFFFlush(tif);
            TIFFForceStrileArrayWriting(tif);
            TIFFFlush(tif);

            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)((rd32(Data, Size, 40) % 1024) + 1));
            TIFFSetupStrips(tif);
            TIFFFlush(tif);

            TIFFClose(tif);
            free(bufa);
            free(bufb);
        }
    }

    return 0;
}