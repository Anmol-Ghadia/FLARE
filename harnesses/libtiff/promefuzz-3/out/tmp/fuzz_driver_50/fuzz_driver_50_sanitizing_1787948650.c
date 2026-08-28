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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static tmsize_t clamp_tmsize(uint32_t v, tmsize_t maxv) {
    tmsize_t tv = (tmsize_t)(v % (uint32_t)(maxv > 0 ? maxv : 1));
    return tv;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w8");
        if (!tif) return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, planar = PLANARCONFIG_CONTIG, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint32_t rowsperstrip = 1;
    uint32_t tilew = 16, tileh = 16;
    int make_tiled = 0;

    if (Size >= 1) width = (uint32_t)(1 + (Data[0] % 64));
    if (Size >= 2) height = (uint32_t)(1 + (Data[1] % 64));
    if (Size >= 3) spp = (uint16_t)(1 + (Data[2] % 4));
    if (Size >= 4) bps = (uint16_t)(1u << (Data[3] % 4)); /* 1,2,4,8 */
    if (Size >= 5) planar = (Data[4] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    if (Size >= 6) photo = (spp >= 3 && (Data[5] & 1)) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    if (Size >= 7) rowsperstrip = 1 + (uint32_t)(Data[6] % (height ? height : 1));
    if (Size >= 8) make_tiled = Data[7] & 1;
    if (Size >= 10) {
        tilew = 1 + (uint32_t)(Data[8] % 32);
        tileh = 1 + (uint32_t)(Data[9] % 32);
    }
    if (Size >= 11) {
        switch (Data[10] % 3) {
            case 0: compression = COMPRESSION_NONE; break;
            case 1: compression = COMPRESSION_LZW; break;
            default: compression = COMPRESSION_PACKBITS; break;
        }
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

    if (make_tiled) {
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tilew);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, tileh);
    } else {
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }

    (void)TIFFSetupStrips(tif);

    {
        void *mem = NULL;
        if (Size >= 15) {
            tmsize_t a = clamp_tmsize(rd32(Data + 11), 1 << 16);
            tmsize_t b = clamp_tmsize(rd32(Data + 11), 1 << 17);
            mem = _TIFFrealloc(NULL, a);
            mem = _TIFFrealloc(mem, b);
            mem = _TIFFrealloc(mem, 0);
        }
        free(mem);
    }

    {
        int use_custom = (Size >= 16) ? (Data[15] & 1) : 0;
        tmsize_t sz = -1;
        void *buf = NULL;
        if (Size >= 20) {
            sz = (tmsize_t)((int32_t)rd32(Data + 16));
            if (sz < -1) sz = -1;
            if (sz > (1 << 20)) sz = (1 << 20);
        }
        if (use_custom && sz > 0) {
            buf = malloc((size_t)sz);
            if (buf && Size > 20) {
                size_t copy = (size_t)sz;
                if (copy > Size - 20) copy = Size - 20;
                memcpy(buf, Data + 20, copy);
            }
        }
        (void)TIFFWriteBufferSetup(tif, buf, sz);
        if (buf) free(buf);
    }

    {
        int use_custom = (Size >= 21) ? (Data[20] & 1) : 0;
        tmsize_t sz = 0;
        void *buf = NULL;
        if (Size >= 25) {
            sz = (tmsize_t)((int32_t)rd32(Data + 21));
            if (sz < 0) sz = -1;
            if (sz > (1 << 20)) sz = (1 << 20);
        }
        if (use_custom && sz > 0) {
            buf = calloc(1, (size_t)sz);
        }
        (void)TIFFReadBufferSetup(tif, buf, sz);
        if (buf) free(buf);
    }

    {
        tmsize_t payload_len = 0;
        const uint8_t *payload = Data;
        if (Size > 32) {
            payload = Data + 32;
            payload_len = (tmsize_t)(Size - 32);
        } else {
            payload_len = (tmsize_t)Size;
        }

        if (make_tiled) {
            uint32_t tile = 0;
            if (Size >= 29) tile = rd32(Data + 25);
            tmsize_t cc = payload_len;
            if (Size >= 33) {
                cc = (tmsize_t)((int32_t)rd32(Data + 29));
                if (cc < 0) cc = payload_len;
                if (cc > payload_len) cc = payload_len;
            }
            (void)TIFFWriteEncodedTile(tif, tile, (void *)payload, cc);
            if (payload_len > 0) {
                (void)TIFFWriteEncodedTile(tif, 0, (void *)payload, payload_len);
            }
        } else {
            uint32_t strip = 0;
            if (Size >= 29) strip = rd32(Data + 25);
            tmsize_t cc = payload_len;
            if (Size >= 33) {
                cc = (tmsize_t)((int32_t)rd32(Data + 29));
                if (cc < 0) cc = payload_len;
                if (cc > payload_len) cc = payload_len;
            }
            (void)TIFFWriteRawStrip(tif, strip, (void *)payload, cc);
            if (payload_len > 0) {
                (void)TIFFWriteRawStrip(tif, 0, (void *)payload, payload_len);
            }
        }
    }

    TIFFClose(tif);
    return 0;
}