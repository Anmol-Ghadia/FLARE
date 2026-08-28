// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateEXIFDirectory at tif_dir.c:1399:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;

    if (Size > 0)
    {
        (void)fwrite(Data, 1, Size, fp);
    }
    else
    {
        static const unsigned char minimal_tiff[] = {
            'I', 'I', 42, 0, 8, 0, 0, 0, 0, 0
        };
        (void)fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_61(const uint8_t *Data, size_t Size)
{
    uint64_t diroff = 0;
    toff_t read_off = 0;

    write_dummy_file(Data, Size);

    if (Size >= sizeof(read_off))
        memcpy(&read_off, Data, sizeof(read_off));
    else if (Size > 0)
        memcpy(&read_off, Data, Size);

    TIFF *tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw)
    {
        (void)TIFFCreateGPSDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        (void)TIFFCreateEXIFDirectory(tif_rw);
        (void)TIFFWriteCustomDirectory(tif_rw, &diroff);
        TIFFFreeDirectory(tif_rw);

        if (diroff != 0)
        {
            (void)TIFFReadCustomDirectory(tif_rw, (toff_t)diroff, NULL);
            TIFFFreeDirectory(tif_rw);
        }

        TIFFClose(tif_rw);
    }

    TIFF *tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro)
    {
        (void)TIFFCreateGPSDirectory(tif_ro);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFCreateEXIFDirectory(tif_ro);
        TIFFFreeDirectory(tif_ro);

        (void)TIFFReadCustomDirectory(tif_ro, read_off, NULL);
        TIFFFreeDirectory(tif_ro);

        TIFFClose(tif_ro);
    }

    return 0;
}