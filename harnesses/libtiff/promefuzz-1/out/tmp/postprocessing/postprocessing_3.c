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

#ifndef TIFFTAG_IMAGEWIDTH
#define TIFFTAG_IMAGEWIDTH 256
#endif
#ifndef TIFFTAG_IMAGELENGTH
#define TIFFTAG_IMAGELENGTH 257
#endif
#ifndef TIFFTAG_BITSPERSAMPLE
#define TIFFTAG_BITSPERSAMPLE 258
#endif
#ifndef TIFFTAG_COMPRESSION
#define TIFFTAG_COMPRESSION 259
#endif
#ifndef TIFFTAG_PHOTOMETRIC
#define TIFFTAG_PHOTOMETRIC 262
#endif
#ifndef TIFFTAG_SAMPLESPERPIXEL
#define TIFFTAG_SAMPLESPERPIXEL 277
#endif
#ifndef TIFFTAG_ROWSPERSTRIP
#define TIFFTAG_ROWSPERSTRIP 278
#endif
#ifndef TIFFTAG_PLANARCONFIG
#define TIFFTAG_PLANARCONFIG 284
#endif
#ifndef TIFFTAG_EXIFIFD
#define TIFFTAG_EXIFIFD 34665
#endif
#ifndef TIFFTAG_SUBFILETYPE
#define TIFFTAG_SUBFILETYPE 254
#endif

#ifndef COMPRESSION_NONE
#define COMPRESSION_NONE 1
#endif
#ifndef PHOTOMETRIC_MINISBLACK
#define PHOTOMETRIC_MINISBLACK 1
#endif
#ifndef PLANARCONFIG_CONTIG
#define PLANARCONFIG_CONTIG 1
#endif

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static const TIFFFieldArray *get_exif_field_array(void) {
#ifdef tiffFieldArray
    return &tiffFieldArray[EXIFFieldArray];
#else
    return NULL;
#endif
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    TIFF *tif2 = NULL;

    uint64_t custom_dir_offset = 0;
    toff_t exif_off = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    const TIFFFieldArray *infoarray = get_exif_field_array();

    if (tif != NULL) {
        TIFFFreeDirectory(tif);

        if (infoarray != NULL) {
            TIFFCreateCustomDirectory(tif, infoarray);

            {
                uint32_t v1 = 0;
                uint32_t v2 = 0;
                if (Size >= 4) {
                    v1 = ((uint32_t)Data[0] << 8) | Data[1 % Size];
                    v2 = ((uint32_t)Data[2 % Size] << 8) | Data[3 % Size];
                }
                (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, v1);
                (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, v2);
            }

            (void)TIFFWriteCustomDirectory(tif, &custom_dir_offset);
        }

        (void)TIFFSetDirectory(tif, 0);

        {
            uint32_t w = (Size > 0) ? (uint32_t)(Data[0] + 1) : 1;
            uint32_t h = (Size > 1) ? (uint32_t)(Data[1] + 1) : 1;
            (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w);
            (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
        }

        TIFFClose(tif);
        tif = NULL;
    }

    tif2 = TIFFOpen("./dummy_file", "r");
    if (tif2 != NULL) {
        (void)TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &height);

        (void)TIFFReadEXIFDirectory(tif2, exif_off);
        (void)TIFFGetField(tif2, TIFFTAG_EXIFIFD, &exif_off);

        if (infoarray != NULL) {
            (void)TIFFReadCustomDirectory(tif2, (toff_t)custom_dir_offset, infoarray);
        }

        (void)TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &width);
        (void)TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &height);

        TIFFClose(tif2);
        tif2 = NULL;
    }

    if (tif != NULL) {
        TIFFClose(tif);
    }

    return 0;
}