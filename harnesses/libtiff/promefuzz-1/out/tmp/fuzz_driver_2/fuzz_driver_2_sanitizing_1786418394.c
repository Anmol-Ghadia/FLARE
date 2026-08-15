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

#ifndef TIFFTAG_IMAGEWIDTH
#define TIFFTAG_IMAGEWIDTH 256
#endif
#ifndef TIFFTAG_IMAGELENGTH
#define TIFFTAG_IMAGELENGTH 257
#endif
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
#endif

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Data && Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif_w = TIFFOpen("./dummy_file", "w+");
    TIFF *tif_r = NULL;

    if (tif_w) {
        TIFFFieldArray custom_array;
        memset(&custom_array, 0, sizeof(custom_array));

        uint64_t custom_dir_offset = 0;
        uint64_t exif_ifd_offset = 0;
        uint32_t width = 0, length = 0;
        uint16_t dir_index = 0;

        TIFFFreeDirectory(tif_w);
        TIFFCreateCustomDirectory(tif_w, &custom_array);

        if (Size >= 4) {
            width = ((uint32_t)Data[0] << 8) | Data[1];
            length = ((uint32_t)Data[2] << 8) | Data[3];
        } else {
            width = 1;
            length = 1;
        }
        if (width == 0) width = 1;
        if (length == 0) length = 1;

        TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, length);

        TIFFWriteCustomDirectory(tif_w, &custom_dir_offset);

        if (Size > 4) {
            dir_index = (uint16_t)Data[4];
        }
        TIFFSetDirectory(tif_w, dir_index);

        TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, width ^ 1U);
        TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, length ^ 1U);

        TIFFClose(tif_w);
        tif_w = NULL;
    }

    tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r) {
        uint32_t got_width = 0, got_length = 0;
        uint64_t exif_ifd_offset = 0;
        TIFFFieldArray custom_array;
        memset(&custom_array, 0, sizeof(custom_array));

        TIFFGetField(tif_r, TIFFTAG_IMAGEWIDTH, &got_width);
        TIFFGetField(tif_r, TIFFTAG_IMAGELENGTH, &got_length);

        TIFFReadEXIFDirectory(tif_r, (toff_t)exif_ifd_offset);

        TIFFGetField(tif_r, TIFFTAG_EXIFIFD, &exif_ifd_offset);

        TIFFReadCustomDirectory(tif_r, (toff_t)0, &custom_array);

        TIFFGetField(tif_r, TIFFTAG_IMAGEWIDTH, &got_width);
        TIFFGetField(tif_r, TIFFTAG_IMAGELENGTH, &got_length);

        TIFFClose(tif_r);
        tif_r = NULL;
    }

    if (tif_w) {
        TIFFClose(tif_w);
    }
    if (tif_r) {
        TIFFClose(tif_r);
    }

    return 0;
}