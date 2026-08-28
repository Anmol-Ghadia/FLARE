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
    if (!fp) return;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        return 0;
    }

    uint32_t width = 1, height = 1;
    uint16_t samplesperpixel = 1;
    uint16_t bitspersample = 8;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint16_t planarconfig = PLANARCONFIG_CONTIG;
    uint16_t compression = COMPRESSION_NONE;
    uint32_t rowsperstrip = 1;

    if (Size > 0) width = (uint32_t)(Data[0]) + 1;
    if (Size > 1) height = (uint32_t)(Data[1]) + 1;
    if (Size > 2) samplesperpixel = (uint16_t)((Data[2] % 4) + 1);
    if (Size > 3) {
        switch (Data[3] % 4) {
            case 0: bitspersample = 1; break;
            case 1: bitspersample = 8; break;
            case 2: bitspersample = 16; break;
            default: bitspersample = 32; break;
        }
    }
    if (Size > 4) {
        photometric = (Data[4] & 1) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    }
    if (photometric == PHOTOMETRIC_RGB && samplesperpixel < 3) {
        samplesperpixel = 3;
    }
    if (Size > 5) {
        planarconfig = (Data[5] & 1) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    }
    if (Size > 6) {
        compression = (Data[6] & 1) ? COMPRESSION_LZW : COMPRESSION_NONE;
    }
    if (Size > 7) {
        rowsperstrip = ((uint32_t)Data[7] % height) + 1;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline <= 0) {
        scanline = (tmsize_t)(((uint64_t)width * samplesperpixel * bitspersample + 7) / 8);
        if (scanline <= 0) scanline = 1;
    }

    uint8_t buf_stack[4096];
    uint8_t *buf = buf_stack;
    tmsize_t bufsize = scanline;
    if (bufsize > (tmsize_t)sizeof(buf_stack)) {
        bufsize = sizeof(buf_stack);
    }
    memset(buf, 0, (size_t)bufsize);
    if (Size > 8 && bufsize > 0) {
        size_t copy = Size - 8;
        if (copy > (size_t)bufsize) copy = (size_t)bufsize;
        memcpy(buf, Data + 8, copy);
    }

    uint32_t row = 0;
    uint16_t sample = 0;
    if (Size > 8) row = (height == 0) ? 0 : ((uint32_t)Data[8] % height);
    if (Size > 9 && planarconfig == PLANARCONFIG_SEPARATE && samplesperpixel > 0) {
        sample = (uint16_t)(Data[9] % samplesperpixel);
    }

    TIFFWriteScanline(tif, buf, row, sample);
    TIFFWriteDirectory(tif);

    if (TIFFCreateEXIFDirectory(tif)) {
        uint16_t exifver[4] = { '0', '2', '3', '1' };
        if (Size > 13) {
            exifver[0] = Data[10];
            exifver[1] = Data[11];
            exifver[2] = Data[12];
            exifver[3] = Data[13];
        }
        TIFFSetField(tif, EXIFTAG_EXIFVERSION, exifver);

        uint64_t diroff = 0;
        TIFFWriteCustomDirectory(tif, &diroff);
    }

    TIFFClose(tif);
    return 0;
}