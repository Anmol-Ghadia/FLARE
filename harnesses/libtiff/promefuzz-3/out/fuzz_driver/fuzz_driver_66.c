// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 2 > n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 4 > n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t rd64(const uint8_t *p, size_t n, size_t off) {
    if (off + 8 > n) return 0;
    return (uint64_t)p[off] |
           ((uint64_t)p[off + 1] << 8) |
           ((uint64_t)p[off + 2] << 16) |
           ((uint64_t)p[off + 3] << 24) |
           ((uint64_t)p[off + 4] << 32) |
           ((uint64_t)p[off + 5] << 40) |
           ((uint64_t)p[off + 6] << 48) |
           ((uint64_t)p[off + 7] << 56);
}

static toff_t pick_offset(const uint8_t *Data, size_t Size, size_t selector) {
    switch (selector % 8) {
        case 0:
            return (toff_t)0;
        case 1:
            return (toff_t)8;
        case 2:
            return (toff_t)rd16(Data, Size, 0);
        case 3:
            return (toff_t)rd32(Data, Size, 0);
        case 4:
            return (toff_t)rd64(Data, Size, 0);
        case 5:
            return (toff_t)rd16(Data, Size, 2);
        case 6:
            return (toff_t)rd32(Data, Size, 4);
        default:
            return (toff_t)(Size > 0 ? (Size - 1) : 0);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif_r = NULL;
    TIFF *tif_rw = NULL;
    toff_t off1, off2, off3, off4;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    off1 = pick_offset(Data, Size, 0);
    off2 = pick_offset(Data, Size, 1);
    off3 = pick_offset(Data, Size, 2);
    off4 = pick_offset(Data, Size, 3);

    tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r) {
        (void)TIFFSetSubDirectory(tif_r, off1);
        (void)TIFFSetSubDirectory(tif_r, off2);
        (void)TIFFSetSubDirectory(tif_r, off3);

        (void)TIFFReadEXIFDirectory(tif_r, off1);
        (void)TIFFReadEXIFDirectory(tif_r, off4);

        (void)TIFFReadGPSDirectory(tif_r, off2);
        (void)TIFFReadGPSDirectory(tif_r, off3);

        TIFFClose(tif_r);
        tif_r = NULL;
    }

    tif_rw = TIFFOpen("./dummy_file", "r+");
    if (tif_rw) {
        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFCreateGPSDirectory(tif_rw);

        (void)TIFFReadEXIFDirectory(tif_rw, off1);
        (void)TIFFReadGPSDirectory(tif_rw, off2);
        (void)TIFFSetSubDirectory(tif_rw, off3);

        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFCreateEXIFDirectory(tif_rw);

        (void)TIFFReadGPSDirectory(tif_rw, off4);
        (void)TIFFReadEXIFDirectory(tif_rw, off3);

        TIFFClose(tif_rw);
        tif_rw = NULL;
    }

    return 0;
}