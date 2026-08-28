// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"

static void noop_extender(TIFF *tif)
{
    (void)tif;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif;
    TIFF *tif_r;
    TIFFExtendProc old_ext;
    uint64_t diroff = 0;
    toff_t off;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    old_ext = TIFFSetTagExtender(noop_extender);

    tif = TIFFOpen("./dummy_file", "w8");
    if (tif)
    {
        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff);
        TIFFFreeDirectory(tif);

        (void)TIFFCreateEXIFDirectory(tif);
        (void)TIFFWriteCustomDirectory(tif, &diroff);
        TIFFFreeDirectory(tif);

        TIFFClose(tif);
    }

    tif_r = TIFFOpen("./dummy_file", "r");
    if (tif_r)
    {
        off = 0;
        (void)TIFFReadCustomDirectory(tif_r, off, NULL);
        TIFFFreeDirectory(tif_r);

        off = (toff_t)diroff;
        (void)TIFFReadCustomDirectory(tif_r, off, NULL);
        TIFFFreeDirectory(tif_r);

        off = (toff_t)Size;
        (void)TIFFReadCustomDirectory(tif_r, off, NULL);
        TIFFFreeDirectory(tif_r);

        off = 0;
        for (i = 0; i < Size && i < sizeof(toff_t); ++i)
            off = (off << 8) | Data[i];
        (void)TIFFReadCustomDirectory(tif_r, off, NULL);
        TIFFFreeDirectory(tif_r);

        TIFFClose(tif_r);
    }

    (void)TIFFSetTagExtender(old_ext);
    return 0;
}