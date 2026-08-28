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
    if (Data && Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char tiny_tiff[] = {
            0x49, 0x49, 0x2A, 0x00,
            0x08, 0x00, 0x00, 0x00,
            0x00, 0x00
        };
        fwrite(tiny_tiff, 1, sizeof(tiny_tiff), fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *Data, size_t Size, size_t off) {
    if (off + 1 >= Size) return 0;
    return (uint16_t)(Data[off] | ((uint16_t)Data[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *Data, size_t Size, size_t off) {
    if (off + 3 >= Size) return 0;
    return (uint32_t)Data[off] |
           ((uint32_t)Data[off + 1] << 8) |
           ((uint32_t)Data[off + 2] << 16) |
           ((uint32_t)Data[off + 3] << 24);
}

static void set_basic_tags(TIFF *tif, const uint8_t *Data, size_t Size, int tiled) {
    uint32_t width = (rd32(Data, Size, 0) % 64U) + 1U;
    uint32_t height = (rd32(Data, Size, 4) % 64U) + 1U;
    uint16_t spp = (uint16_t)((rd16(Data, Size, 8) % 4U) + 1U);
    uint16_t bps_choices[] = {1, 8, 16, 32};
    uint16_t bps = bps_choices[rd16(Data, Size, 10) % 4U];
    uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t planar = (rd16(Data, Size, 12) & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint16_t sampleformat = SAMPLEFORMAT_UINT;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

    if (tiled) {
        uint32_t tw = ((rd32(Data, Size, 16) % 32U) + 1U);
        uint32_t th = ((rd32(Data, Size, 20) % 32U) + 1U);
        TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
        TIFFSetField(tif, TIFFTAG_TILELENGTH, th);
    } else {
        uint32_t rowsperstrip = ((rd32(Data, Size, 24) % 64U) + 1U);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif) {
            uint16_t dirn1 = rd16(Data, Size, 0);
            uint16_t dirn2 = rd16(Data, Size, 2);

            (void)TIFFSetDirectory(tif, dirn1);
            (void)TIFFSetDirectory(tif, dirn2);
            TIFFClose(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r+");
        if (tif) {
            uint16_t dirn = (uint16_t)((rd16(Data, Size, 4) % 8U) + 1U);
            (void)TIFFUnlinkDirectory(tif, dirn);
            (void)TIFFSetDirectory(tif, rd16(Data, Size, 6));
            TIFFClose(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w");
        if (tif) {
            int tiled = (Size > 0) ? (Data[0] & 1) : 0;
            set_basic_tags(tif, Data, Size, tiled);

            (void)TIFFWriteCheck(tif, tiled ? 1 : 0, "fuzz-writecheck");
            (void)TIFFCheckpointDirectory(tif);
            (void)TIFFWriteDirectory(tif);
            (void)TIFFRewriteDirectory(tif);

            if (Size > 1) {
                set_basic_tags(tif, Data + 1, Size - 1, !tiled);
                (void)TIFFWriteCheck(tif, (!tiled) ? 1 : 0, "fuzz-writecheck-2");
                (void)TIFFCheckpointDirectory(tif);
                (void)TIFFWriteDirectory(tif);
                (void)TIFFRewriteDirectory(tif);
            }

            TIFFClose(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "w8");
        if (tif) {
            int tiled = (Size > 2) ? (Data[2] & 1) : 0;
            set_basic_tags(tif, Data, Size, tiled);

            (void)TIFFWriteCheck(tif, tiled ? 1 : 0, "fuzz-bigtiff");
            (void)TIFFCheckpointDirectory(tif);
            (void)TIFFWriteDirectory(tif);
            (void)TIFFRewriteDirectory(tif);

            TIFFClose(tif);
        }
    }

    {
        TIFF *tif = TIFFOpen("./dummy_file", "a");
        if (tif) {
            int tiled = (Size > 3) ? (Data[3] & 1) : 0;
            set_basic_tags(tif, Data, Size, tiled);

            (void)TIFFWriteCheck(tif, tiled ? 1 : 0, "fuzz-append");
            (void)TIFFCheckpointDirectory(tif);
            (void)TIFFWriteDirectory(tif);
            (void)TIFFRewriteDirectory(tif);
            (void)TIFFSetDirectory(tif, rd16(Data, Size, 8));
            (void)TIFFUnlinkDirectory(tif, (uint16_t)((rd16(Data, Size, 10) % 8U) + 1U));

            TIFFClose(tif);
        }
    }

    return 0;
}