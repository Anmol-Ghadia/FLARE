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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *Data, size_t Size, size_t off, uint16_t defv) {
    if (off + 1 >= Size) return defv;
    return (uint16_t)(Data[off] | ((uint16_t)Data[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *Data, size_t Size, size_t off, uint32_t defv) {
    if (off + 3 >= Size) return defv;
    return (uint32_t)Data[off] |
           ((uint32_t)Data[off + 1] << 8) |
           ((uint32_t)Data[off + 2] << 16) |
           ((uint32_t)Data[off + 3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w+");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
    }
    if (!tif) {
        return 0;
    }

    uint32_t width = rd32(Data, Size, 0, 1) % 64 + 1;
    uint32_t height = rd32(Data, Size, 4, 0) % 64 + 1;
    uint16_t spp = (uint16_t)(rd16(Data, Size, 8, 1) % 4 + 1);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16};
    uint16_t bps = bps_choices[rd16(Data, Size, 10, 3) % 5];
    uint16_t photo = (rd16(Data, Size, 12, 0) & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (rd16(Data, Size, 14, 0) & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = rd32(Data, Size, 16, height) % (height + 1);
    if (rowsperstrip == 0) rowsperstrip = 1;

    if (photo == PHOTOMETRIC_MINISBLACK) spp = 1;
    if (photo == PHOTOMETRIC_RGB && spp < 3) spp = 3;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    (void)TIFFSetupStrips(tif);
    (void)TIFFFlush(tif);
    (void)TIFFForceStrileArrayWriting(tif);
    (void)TIFFCheckpointDirectory(tif);

    if (Size > 20) {
        (void)TIFFWriteDirectory(tif);

        width = rd32(Data, Size, 20, 2) % 32 + 1;
        height = rd32(Data, Size, 24, 2) % 32 + 1;
        spp = 1;
        bps = 8;
        photo = PHOTOMETRIC_MINISBLACK;
        rowsperstrip = rd32(Data, Size, 28, height) % (height + 1);
        if (rowsperstrip == 0) rowsperstrip = 1;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        (void)TIFFSetupStrips(tif);
        (void)TIFFCheckpointDirectory(tif);
        (void)TIFFFlush(tif);
        (void)TIFFForceStrileArrayWriting(tif);
        (void)TIFFWriteDirectory(tif);
    }

    TIFFSetField(tif, TIFFTAG_SOFTWARE, "fuzz");
    (void)TIFFRewriteDirectory(tif);
    (void)TIFFFlush(tif);

    TIFFClose(tif);
    return 0;
}