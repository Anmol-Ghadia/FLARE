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

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static uint32_t rd32(const uint8_t *p, size_t n, uint32_t defval) {
    if (n < 4)
        return defval;
    return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        (void)TIFFIsTiled(tif);
        (void)TIFFFlush(tif);
        TIFFClose(tif);
    }

    TIFF *tw = TIFFOpen("./dummy_file", "w");
    if (tw) {
        uint32_t width = 1, height = 1;
        uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
        uint16_t planar = PLANARCONFIG_CONTIG;
        uint16_t compression = COMPRESSION_NONE;
        uint16_t sampleformat = SAMPLEFORMAT_UINT;
        uint32_t rowsperstrip = 1;
        int make_tiled = 0;

        if (Size >= 1)
            make_tiled = Data[0] & 1;
        if (Size >= 5)
            width = (rd32(Data + 1, Size - 1, 1) % 64) + 1;
        if (Size >= 9)
            height = (rd32(Data + 5, Size - 5, 1) % 64) + 1;
        if (Size >= 10)
            spp = (uint16_t)((Data[9] % 4) + 1);
        if (Size >= 11)
            bps = (Data[10] & 1) ? 8 : 16;
        if (Size >= 12)
            photo = (Data[11] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        if (photo == PHOTOMETRIC_RGB && spp < 3)
            spp = 3;
        if (Size >= 16)
            rowsperstrip = (rd32(Data + 12, Size - 12, 1) % height) + 1;

        (void)TIFFSetField(tw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tw, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tw, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tw, TIFFTAG_SAMPLEFORMAT, sampleformat);

        if (make_tiled) {
            uint32_t twidth = 16, theight = 16;
            if (Size >= 20)
                twidth = (rd32(Data + 16, Size - 16, 16) % 32) + 1;
            if (Size >= 24)
                theight = (rd32(Data + 20, Size - 20, 16) % 32) + 1;
            (void)TIFFSetField(tw, TIFFTAG_TILEWIDTH, twidth);
            (void)TIFFSetField(tw, TIFFTAG_TILELENGTH, theight);
        } else {
            (void)TIFFSetField(tw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        }

        (void)TIFFIsTiled(tw);
        (void)TIFFCheckpointDirectory(tw);
        (void)TIFFFlush(tw);
        (void)TIFFForceStrileArrayWriting(tw);
        (void)TIFFRewriteDirectory(tw);
        (void)TIFFWriteDirectory(tw);

        if (Size > 24) {
            uint32_t width2 = (rd32(Data + 24, Size - 24, 1) % 32) + 1;
            uint32_t height2 = (Size > 28) ? ((rd32(Data + 28, Size - 28, 1) % 32) + 1) : 1;

            (void)TIFFSetField(tw, TIFFTAG_IMAGEWIDTH, width2);
            (void)TIFFSetField(tw, TIFFTAG_IMAGELENGTH, height2);
            (void)TIFFSetField(tw, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)1);
            (void)TIFFSetField(tw, TIFFTAG_BITSPERSAMPLE, (uint16_t)8);
            (void)TIFFSetField(tw, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG);
            (void)TIFFSetField(tw, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);
            (void)TIFFSetField(tw, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_MINISBLACK);
            (void)TIFFSetField(tw, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);

            (void)TIFFCheckpointDirectory(tw);
            (void)TIFFFlush(tw);
            (void)TIFFWriteDirectory(tw);
            (void)TIFFRewriteDirectory(tw);
            (void)TIFFForceStrileArrayWriting(tw);
        }

        TIFFClose(tw);
    }

    TIFF *trw = TIFFOpen("./dummy_file", "r+");
    if (trw) {
        (void)TIFFIsTiled(trw);
        (void)TIFFFlush(trw);
        (void)TIFFCheckpointDirectory(trw);
        (void)TIFFForceStrileArrayWriting(trw);
        (void)TIFFRewriteDirectory(trw);
        (void)TIFFWriteDirectory(trw);
        TIFFClose(trw);
    }

    remove("./dummy_file");
    return 0;
}