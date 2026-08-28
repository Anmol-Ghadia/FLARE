// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetVersion at tif_version.c:29:1 in tiffio.h
// TIFFGetVersion at tif_version.c:29:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static void exercise_read_modes(void) {
    static const char *modes[] = {
        "r", "rm", "rC", "rc", "rh", "r+", "rb", "r8"
    };
    size_t i, j;
    const char *ver = TIFFGetVersion();
    (void)ver;

    for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        TIFF *tif = TIFFOpen("./dummy_file", modes[i]);
        if (!tif)
            continue;

        (void)TIFFSetDirectory(tif, 0);
        (void)TIFFSetDirectory(tif, 1);
        (void)TIFFSetDirectory(tif, 0xFFFFu);

        (void)TIFFCreateDirectory(tif);
        (void)TIFFWriteDirectory(tif);
        TIFFFreeDirectory(tif);

        for (j = 0; j < 4; j++) {
            (void)TIFFSetDirectory(tif, (uint16_t)j);
        }

        TIFFClose(tif);
    }
}

static void exercise_write_modes(const uint8_t *Data, size_t Size) {
    static const char *modes[] = {
        "w", "w8", "w+", "a", "a8"
    };
    size_t i;

    for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        TIFF *tif = TIFFOpen("./dummy_file", modes[i]);
        if (!tif)
            continue;

        (void)TIFFCreateDirectory(tif);

        if (Size >= 8) {
            uint32_t width = ((uint32_t)Data[0] << 8) | Data[1];
            uint32_t height = ((uint32_t)Data[2] << 8) | Data[3];
            uint16_t spp = (uint16_t)((Data[4] % 4) + 1);
            uint16_t bps = (uint16_t)(1u << (Data[5] % 4)); /* 1,2,4,8 */
            uint16_t photo = (uint16_t)((Data[6] % 2) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
            uint16_t planar = (uint16_t)((Data[7] % 2) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG);

            if (width == 0) width = 1;
            if (height == 0) height = 1;

            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
            TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
            TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
        }

        (void)TIFFWriteDirectory(tif);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateDirectory(tif);
        (void)TIFFWriteDirectory(tif);
        (void)TIFFSetDirectory(tif, 0);
        (void)TIFFSetDirectory(tif, 1);

        TIFFClose(tif);
    }
}

int LLVMFuzzerTestOneInput_72(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);
    (void)TIFFGetVersion();
    exercise_read_modes();
    exercise_write_modes(Data, Size);
    return 0;
}