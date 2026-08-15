// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
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

static uint16_t read_u16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t read_u32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static uint64_t read_u64(const uint8_t *p, size_t n, size_t off) {
    uint64_t lo = read_u32(p, n, off);
    uint64_t hi = read_u32(p, n, off + 4);
    return lo | (hi << 32);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "rm");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
        if (!tif) {
            return 0;
        }
    }

    uint16_t dir_index_a = read_u16(Data, Size, 0);
    uint16_t dir_index_b = read_u16(Data, Size, 2);

    uint64_t off_a = read_u64(Data, Size, 4);
    uint64_t off_b = read_u64(Data, Size, 12);
    uint64_t off_c = read_u64(Data, Size, 20);

    uint32_t selector = read_u32(Data, Size, 28);

    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFSetDirectory(tif, dir_index_a);
    (void)TIFFSetDirectory(tif, dir_index_b);

    (void)TIFFSetSubDirectory(tif, off_a);
    (void)TIFFSetSubDirectory(tif, off_b);

    (void)TIFFReadEXIFDirectory(tif, off_a);
    (void)TIFFReadEXIFDirectory(tif, off_c);

    (void)TIFFReadGPSDirectory(tif, off_b);
    (void)TIFFReadGPSDirectory(tif, off_c);

    TIFFFreeDirectory(tif);

    if ((selector & 1U) != 0) {
        (void)TIFFSetDirectory(tif, dir_index_a ^ dir_index_b);
    }
    if ((selector & 2U) != 0) {
        (void)TIFFSetSubDirectory(tif, off_a ^ off_b);
    }
    if ((selector & 4U) != 0) {
        (void)TIFFReadEXIFDirectory(tif, off_a ^ off_c);
    }
    if ((selector & 8U) != 0) {
        (void)TIFFReadGPSDirectory(tif, off_b ^ off_c);
    }

    {
        const TIFFFieldArray *info = NULL;
        (void)TIFFReadCustomDirectory(tif, off_a, info);
        (void)TIFFReadCustomDirectory(tif, off_b, info);
    }

    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    return 0;
}