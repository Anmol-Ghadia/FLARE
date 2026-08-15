#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

#ifndef TIFF_ARRAYCOUNT
#define TIFF_ARRAYCOUNT(x) (sizeof(x) / sizeof((x)[0]))
#endif

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

    if (Data != NULL && Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I','I', 42,0, 8,0,0,0, 0,0
        };
        (void)fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }

    fclose(fp);
}

static const TIFFFieldArray *get_exif_field_array(void) {
    const TIFFField *f = TIFFFieldWithTag(NULL, TIFFTAG_EXIFIFD);
    if (f == NULL) {
        return NULL;
    }
    return TIFFFieldSetGetSize(f) ? NULL : NULL;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint32_t width = 1;
    uint32_t length = 1;
    uint16_t dir_index = 0;
    uint64_t custom_dir_offset = 0;
    uint64_t exif_ifd_offset = 0;
    uint32_t got_width = 0;
    uint32_t got_length = 0;

    write_input_file(Data, Size);

    if (Size >= 2) {
        width = (uint32_t)Data[0] | ((uint32_t)Data[1] << 8);
        if (width == 0) {
            width = 1;
        }
    }
    if (Size >= 4) {
        length = (uint32_t)Data[2] | ((uint32_t)Data[3] << 8);
        if (length == 0) {
            length = 1;
        }
    }
    if (Size >= 5) {
        dir_index = (uint16_t)Data[4];
    }
    if (Size >= 13) {
        exif_ifd_offset = ((uint64_t)Data[5]) |
                          ((uint64_t)Data[6] << 8) |
                          ((uint64_t)Data[7] << 16) |
                          ((uint64_t)Data[8] << 24) |
                          ((uint64_t)Data[9] << 32) |
                          ((uint64_t)Data[10] << 40) |
                          ((uint64_t)Data[11] << 48) |
                          ((uint64_t)Data[12] << 56);
    }

    TIFF *tif_w = TIFFOpen("./dummy_file", "w+");
    if (tif_w != NULL) {
        TIFFFreeDirectory(tif_w);

        /* Use a non-NULL field array to avoid NULL dereference in TIFFCreateCustomDirectory. */
        const TIFFFieldArray *custom_info = _TIFFGetExifFields();
        if (custom_info != NULL) {
            TIFFCreateCustomDirectory(tif_w, custom_info);
            TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, width);
            TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, length);
            TIFFWriteCustomDirectory(tif_w, &custom_dir_offset);
            TIFFSetDirectory(tif_w, dir_index);
            TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, width ^ 1U);
            TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, length ^ 1U);
        }
        TIFFClose(tif_w);
        tif_w = NULL;
    }

    TIFF *tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r != NULL) {
        TIFFGetField(tif_r, TIFFTAG_IMAGEWIDTH, &got_width);
        TIFFGetField(tif_r, TIFFTAG_IMAGELENGTH, &got_length);
        TIFFReadEXIFDirectory(tif_r, (toff_t)exif_ifd_offset);
        TIFFGetField(tif_r, TIFFTAG_EXIFIFD, &exif_ifd_offset);
        TIFFReadCustomDirectory(tif_r, (toff_t)custom_dir_offset, _TIFFGetExifFields());
        TIFFGetField(tif_r, TIFFTAG_IMAGEWIDTH, &got_width);
        TIFFGetField(tif_r, TIFFTAG_IMAGELENGTH, &got_length);
        TIFFClose(tif_r);
        tif_r = NULL;
    }

    if (tif_w != NULL) {
        TIFFClose(tif_w);
    }
    if (tif_r != NULL) {
        TIFFClose(tif_r);
    }

    return 0;
}