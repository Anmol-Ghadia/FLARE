#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void set_basic_image_tags(TIFF *tif, const uint8_t *Data, size_t Size) {
    uint32_t width = 1, height = 1;
    uint16_t spp = 1, bps = 8, photo = PHOTOMETRIC_MINISBLACK;
    uint16_t planar = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = ORIENTATION_TOPLEFT;
    uint32_t rowsperstrip = 1;
    unsigned char pixel = 0;

    if (Size >= 1) width = (uint32_t)(Data[0] % 32u) + 1u;
    if (Size >= 2) height = (uint32_t)(Data[1] % 32u) + 1u;
    if (Size >= 3) spp = (uint16_t)((Data[2] % 4u) + 1u);
    if (Size >= 4) {
        uint8_t v = (uint8_t)(Data[3] % 3u);
        bps = (v == 0) ? 8 : (v == 1 ? 16 : 32);
    }
    if (Size >= 5) {
        uint8_t v = (uint8_t)(Data[4] % 3u);
        photo = (v == 0) ? PHOTOMETRIC_MINISBLACK :
                (v == 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISWHITE;
    }
    if (Size >= 6) {
        compression = (Data[5] & 1u) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size >= 7) {
        rowsperstrip = (uint32_t)(Data[6] % height) + 1u;
    }
    if (Size >= 8) {
        orientation = (uint16_t)((Data[7] % 8u) + 1u);
    }
    if (Size >= 9) {
        planar = (Data[8] & 1u) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size >= 10) {
        pixel = Data[9];
    }

    if (photo != PHOTOMETRIC_RGB) {
        spp = 1;
    } else if (spp < 3) {
        spp = 3;
    }

    (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);

    if (photo == PHOTOMETRIC_RGB) {
        tsize_t linebytes = (tsize_t)width * spp * (bps / 8);
        if (linebytes > 0 && linebytes < 4096) {
            unsigned char buf[4096];
            size_t i;
            for (i = 0; i < (size_t)linebytes; i++) {
                buf[i] = (unsigned char)(pixel + (unsigned char)i);
            }
            (void)TIFFWriteScanline(tif, buf, 0, 0);
        }
    } else {
        tsize_t linebytes = (tsize_t)width * (bps / 8);
        if (linebytes > 0 && linebytes < 4096) {
            unsigned char buf[4096];
            size_t i;
            for (i = 0; i < (size_t)linebytes; i++) {
                buf[i] = (unsigned char)(pixel ^ (unsigned char)i);
            }
            (void)TIFFWriteScanline(tif, buf, 0, 0);
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    TIFF *tif_read = NULL;
    uint64_t custom_off = 0;
    toff_t off = 0;

    write_input_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "w8");
    if (tif) {
        set_basic_image_tags(tif, Data, Size);
        (void)TIFFWriteDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        if (Size >= 12) {
            uint16_t exifver[4];
            exifver[0] = Data[Size - 4];
            exifver[1] = Data[Size - 3];
            exifver[2] = Data[Size - 2];
            exifver[3] = Data[Size - 1];
            (void)TIFFSetField(tif, EXIFTAG_EXIFVERSION, exifver);
        }
        (void)TIFFWriteCustomDirectory(tif, &custom_off);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteDirectory(tif);
        TIFFFreeDirectory(tif);

        TIFFClose(tif);
        tif = NULL;
    }

    tif_read = TIFFOpen("./dummy_file", "r");
    if (tif_read) {
        (void)TIFFReadDirectory(tif_read);

        off = (toff_t)custom_off;
        (void)TIFFReadCustomDirectory(tif_read, off, NULL);

        if (Size >= sizeof(uint64_t)) {
            uint64_t fuzz_off = 0;
            memcpy(&fuzz_off, Data, sizeof(uint64_t));
            (void)TIFFReadCustomDirectory(tif_read, (toff_t)fuzz_off, NULL);
        }

        (void)TIFFReadCustomDirectory(tif_read, 0, NULL);
        TIFFFreeDirectory(tif_read);

        TIFFClose(tif_read);
        tif_read = NULL;
    }

    return 0;
}