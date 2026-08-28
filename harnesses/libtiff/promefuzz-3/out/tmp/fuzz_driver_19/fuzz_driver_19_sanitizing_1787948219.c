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

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void set_basic_tags(TIFF *tif, const uint8_t *Data, size_t Size) {
    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = 1;

    if (Size > 0) width = (uint32_t)(Data[0]) + 1;
    if (Size > 1) height = (uint32_t)(Data[1]) + 1;
    if (Size > 2) spp = (uint16_t)((Data[2] % 4) + 1);
    if (Size > 3) {
        switch (Data[3] % 4) {
            case 0: bps = 1; break;
            case 1: bps = 4; break;
            case 2: bps = 8; break;
            default: bps = 16; break;
        }
    }
    if (Size > 4) {
        photometric = (Data[4] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    }
    if (Size > 5) {
        planar = (Data[5] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size > 6) {
        rowsperstrip = (uint32_t)(Data[6]) + 1;
    }
    if (Size > 7) {
        switch (Data[7] % 3) {
            case 0: compression = COMPRESSION_NONE; break;
            case 1: compression = COMPRESSION_PACKBITS; break;
            default: compression = COMPRESSION_LZW; break;
        }
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    if (photometric == PHOTOMETRIC_RGB && spp < 3) {
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    }

    if (compression == COMPRESSION_LZW) {
        TIFFSetField(tif, TIFFTAG_PREDICTOR, 1);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint64_t strip_size64 = TIFFStripSize64(tif);
        uint32_t nstrips = TIFFNumberOfStrips(tif);

        (void)TIFFVStripSize64(tif, (uint32_t)-1);
        (void)TIFFVStripSize64(tif, 0);
        (void)TIFFVStripSize64(tif, 1);
        if (Size > 0) {
            (void)TIFFVStripSize64(tif, (uint32_t)Data[0]);
        }

        for (uint32_t i = 0; i < nstrips && i < 8; i++) {
            int err = 0;
            uint64_t bc = TIFFGetStrileByteCountWithErr(tif, i, &err);

            tmsize_t outsize = 0;
            if (!err && bc > 0 && bc < (1U << 20)) {
                outsize = (tmsize_t)bc;
            } else if (strip_size64 > 0 && strip_size64 < (1U << 20)) {
                outsize = (tmsize_t)strip_size64;
            } else {
                outsize = 4096;
            }

            void *outbuf = malloc((size_t)outsize);
            if (outbuf) {
                (void)TIFFReadEncodedStrip(tif, i, outbuf, outsize);
                if (Size > 0) {
                    (void)TIFFReadFromUserBuffer(
                        tif, i, (void *)Data, (tmsize_t)Size, outbuf, outsize);
                }
                free(outbuf);
            }
        }

        if (nstrips == 0) {
            int err = 0;
            (void)TIFFGetStrileByteCountWithErr(tif, 0, &err);
            if (Size > 0) {
                uint8_t stackbuf[256];
                (void)TIFFReadFromUserBuffer(
                    tif, 0, (void *)Data, (tmsize_t)Size, stackbuf, (tmsize_t)sizeof(stackbuf));
                (void)TIFFReadEncodedStrip(tif, 0, stackbuf, (tmsize_t)sizeof(stackbuf));
            }
        }

        TIFFClose(tif);
    }

    TIFF *tw = TIFFOpen("./dummy_file", "w");
    if (tw) {
        set_basic_tags(tw, Data, Size);

        uint64_t strip_size64 = TIFFStripSize64(tw);
        uint32_t nstrips = TIFFNumberOfStrips(tw);

        (void)TIFFVStripSize64(tw, (uint32_t)-1);
        (void)TIFFVStripSize64(tw, 1);
        if (Size > 1) {
            (void)TIFFVStripSize64(tw, (uint32_t)Data[1]);
        }

        for (uint32_t i = 0; i < nstrips && i < 4; i++) {
            int err = 0;
            (void)TIFFGetStrileByteCountWithErr(tw, i, &err);

            tmsize_t outsize = 256;
            if (strip_size64 > 0 && strip_size64 < (1U << 20)) {
                outsize = (tmsize_t)strip_size64;
            }

            void *outbuf = malloc((size_t)outsize);
            if (outbuf) {
                memset(outbuf, 0, (size_t)outsize);
                if (Size > 0) {
                    (void)TIFFReadFromUserBuffer(
                        tw, i, (void *)Data, (tmsize_t)Size, outbuf, outsize);
                }
                (void)TIFFReadEncodedStrip(tw, i, outbuf, outsize);
                free(outbuf);
            }
        }

        TIFFClose(tw);
    }

    return 0;
}