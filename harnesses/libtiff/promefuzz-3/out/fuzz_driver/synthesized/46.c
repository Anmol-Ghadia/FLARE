// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFRewriteDirectory at tif_dirwrite.c:275:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFGetMode at tif_open.c:574:1 in tiffio.h
// TIFFIsUpSampled at tif_open.c:649:1 in tiffio.h
// TIFFSetMode at tif_open.c:583:1 in tiffio.h
// TIFFGetMode at tif_open.c:574:1 in tiffio.h
// TIFFIsUpSampled at tif_open.c:649:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#ifndef O_BINARY
#define O_BINARY _O_BINARY
#endif
#define OPEN_FN _open
#define CLOSE_FN _close
#else
#include <unistd.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#define OPEN_FN open
#define CLOSE_FN close
#endif

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
#ifndef COMPRESSION_NONE
#define COMPRESSION_NONE 1
#endif
#ifndef PHOTOMETRIC_MINISBLACK
#define PHOTOMETRIC_MINISBLACK 1
#endif

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput_46(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    int fd = OPEN_FN("./dummy_file", O_RDWR | O_BINARY);
    if (fd < 0) {
        fd = OPEN_FN("./dummy_file", O_RDONLY | O_BINARY);
        if (fd < 0)
            return 0;
    }

    const char *modes[] = {
        "r", "r+", "w", "w8", "a"
    };

    size_t mode_index = (Size > 0) ? (Data[0] % (sizeof(modes) / sizeof(modes[0]))) : 0;
    TIFF *tif = TIFFFdOpen(fd, "./dummy_file", modes[mode_index]);

    if (!tif) {
        CLOSE_FN(fd);
        return 0;
    }

    (void)TIFFGetMode(tif);
    (void)TIFFIsUpSampled(tif);

    int set_modes[] = {
        0, 1, 2, 3, -1, 0x4000, 0x8000, 0x10000, 0x7fffffff
    };

    for (size_t i = 0; i < sizeof(set_modes) / sizeof(set_modes[0]); i++) {
        (void)TIFFSetMode(tif, set_modes[i]);
        (void)TIFFGetMode(tif);
        (void)TIFFIsUpSampled(tif);
    }

    if (Size >= 8) {
        uint32_t w = (uint32_t)Data[1] + 1;
        uint32_t h = (uint32_t)Data[2] + 1;
        uint16_t bps = (uint16_t)((Data[3] % 32) + 1);
        uint16_t spp = (uint16_t)((Data[4] % 4) + 1);
        uint32_t rps = (uint32_t)Data[5] + 1;
        uint16_t photo = (Data[6] & 1) ? PHOTOMETRIC_MINISBLACK : 0;
        uint16_t compression = (Data[7] & 1) ? COMPRESSION_NONE : 1;

        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rps);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    }

    (void)TIFFWriteCheck(tif, 0, "fuzz_scanline");
    (void)TIFFWriteCheck(tif, 1, "fuzz_tile");
    (void)TIFFRewriteDirectory(tif);

    TIFFClose(tif);
    return 0;
}