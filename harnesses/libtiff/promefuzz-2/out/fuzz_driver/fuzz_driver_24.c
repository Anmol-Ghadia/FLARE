// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
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
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    uint64_t lo = rd32(p, n, off);
    uint64_t hi = rd32(p, n, off + 4);
    return lo | (hi << 32);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r) {
        uint64_t off1 = rd64(Data, Size, 0);
        uint64_t off2 = rd64(Data, Size, 8);
        uint64_t off3 = rd64(Data, Size, 16);
        uint64_t off4 = rd64(Data, Size, 24);

        (void)TIFFSetSubDirectory(tif_r, off1);
        (void)TIFFReadEXIFDirectory(tif_r, (toff_t)off2);
        (void)TIFFReadGPSDirectory(tif_r, (toff_t)off3);
        (void)TIFFReadCustomDirectory(tif_r, (toff_t)off4, NULL);

        TIFFFreeDirectory(tif_r);

        (void)TIFFSetSubDirectory(tif_r, (uint64_t)rd32(Data, Size, 4));
        (void)TIFFReadEXIFDirectory(tif_r, (toff_t)rd32(Data, Size, 12));
        (void)TIFFReadGPSDirectory(tif_r, (toff_t)rd32(Data, Size, 20));
        (void)TIFFReadCustomDirectory(tif_r, (toff_t)rd32(Data, Size, 28), NULL);

        TIFFFreeDirectory(tif_r);
        TIFFClose(tif_r);
    }

    TIFF *tif_w = TIFFOpen("./dummy_file", "w");
    if (tif_w) {
        uint64_t diroff = 0;

        uint32_t width = (uint32_t)(rd16(Data, Size, 0) + 1);
        uint32_t height = (uint32_t)(rd16(Data, Size, 2) + 1);
        uint16_t spp = (uint16_t)((rd16(Data, Size, 4) % 4) + 1);
        uint16_t bps_choices[] = {1, 8, 16, 32};
        uint16_t bps = bps_choices[rd16(Data, Size, 6) % 4];
        uint16_t photo = (uint16_t)(rd16(Data, Size, 8) % 6);
        uint16_t planar = (uint16_t)((rd16(Data, Size, 10) % 2) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG);
        uint16_t sampleformat = (uint16_t)((rd16(Data, Size, 12) % 3) + 1);
        uint32_t rowsperstrip = (uint32_t)(rd16(Data, Size, 14) + 1);

        if (photo == 0) photo = PHOTOMETRIC_MINISWHITE;
        else if (photo == 1) photo = PHOTOMETRIC_MINISBLACK;
        else if (photo == 2) photo = PHOTOMETRIC_RGB;
        else if (photo == 3) photo = PHOTOMETRIC_PALETTE;
        else if (photo == 4) photo = PHOTOMETRIC_MASK;
        else photo = PHOTOMETRIC_SEPARATED;

        (void)TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif_w, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif_w, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif_w, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tif_w, TIFFTAG_SAMPLEFORMAT, sampleformat);
        (void)TIFFSetField(tif_w, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        (void)TIFFSetField(tif_w, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif_w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

        (void)TIFFWriteCustomDirectory(tif_w, &diroff);
        TIFFFreeDirectory(tif_w);

        (void)TIFFSetField(tif_w, TIFFTAG_IMAGEWIDTH, (uint32_t)1);
        (void)TIFFSetField(tif_w, TIFFTAG_IMAGELENGTH, (uint32_t)1);
        (void)TIFFSetField(tif_w, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)1);
        (void)TIFFSetField(tif_w, TIFFTAG_BITSPERSAMPLE, (uint16_t)8);
        (void)TIFFSetField(tif_w, TIFFTAG_PLANARCONFIG, (uint16_t)PLANARCONFIG_CONTIG);
        (void)TIFFSetField(tif_w, TIFFTAG_SAMPLEFORMAT, (uint16_t)SAMPLEFORMAT_UINT);
        (void)TIFFSetField(tif_w, TIFFTAG_ROWSPERSTRIP, (uint32_t)1);
        (void)TIFFSetField(tif_w, TIFFTAG_PHOTOMETRIC, (uint16_t)PHOTOMETRIC_MINISBLACK);
        (void)TIFFSetField(tif_w, TIFFTAG_COMPRESSION, (uint16_t)COMPRESSION_NONE);

        (void)TIFFWriteCustomDirectory(tif_w, &diroff);
        TIFFFreeDirectory(tif_w);
        TIFFClose(tif_w);
    }

    TIFF *tif_rw = TIFFOpen("./dummy_file", "r+");
    if (tif_rw) {
        uint64_t diroff = 0;
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        (void)TIFFSetSubDirectory(tif_rw, diroff);
        (void)TIFFReadEXIFDirectory(tif_rw, (toff_t)diroff);
        (void)TIFFReadGPSDirectory(tif_rw, (toff_t)diroff);
        (void)TIFFReadCustomDirectory(tif_rw, (toff_t)diroff, NULL);
        TIFFFreeDirectory(tif_rw);
        TIFFClose(tif_rw);
    }

    return 0;
}