// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    static const unsigned char minimal_tiff[] = {
        0x49, 0x49, 0x2A, 0x00,
        0x08, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;

    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_68(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r+");
    if (!tif) tif = TIFFOpen("./dummy_file", "r");
    if (!tif) tif = TIFFOpen("./dummy_file", "w+");
    if (!tif) return 0;

    (void)TIFFGetTagListCount(tif);

    (void)TIFFCreateEXIFDirectory(tif);
    (void)TIFFGetTagListCount(tif);

    TIFFFreeDirectory(tif);

    toff_t off0 = 0;
    toff_t off1 = 8;
    toff_t off2 = (toff_t)Size;

    if (Size >= sizeof(toff_t)) {
        memcpy(&off2, Data, sizeof(toff_t));
    } else {
        size_t i;
        for (i = 0; i < Size; ++i) {
            off2 = (off2 << 5) ^ (toff_t)Data[i];
        }
    }

    (void)TIFFReadEXIFDirectory(tif, off0);
    (void)TIFFReadEXIFDirectory(tif, off1);
    (void)TIFFReadEXIFDirectory(tif, off2);

    TIFFFreeDirectory(tif);
    (void)TIFFGetTagListCount(tif);

    TIFFClose(tif);
    return 0;
}