// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFGetField at tif_dir.c:1270:1 in tiffio.h
// TIFFGetMode at tif_open.c:574:1 in tiffio.h
// TIFFIsMSB2LSB at tif_open.c:658:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFReadRGBATileExt at tif_getimage.c:2980:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static int get_u32_tag(TIFF *tif, uint32_t tag, uint32_t *v)
{
    return TIFFGetField(tif, tag, v) ? 1 : 0;
}

static void fuzz_common_queries(TIFF *tif)
{
    char errbuf[1024];
    memset(errbuf, 0, sizeof(errbuf));
    (void)TIFFGetMode(tif);
    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFRGBAImageOK(tif, errbuf);
}

static void fuzz_directories(TIFF *tif)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!TIFFReadDirectory(tif))
            break;
        fuzz_common_queries(tif);
    }
}

static void fuzz_rgba_tile(TIFF *tif)
{
    uint32_t tilew = 0, tileh = 0;
    if (!get_u32_tag(tif, TIFFTAG_TILEWIDTH, &tilew) ||
        !get_u32_tag(tif, TIFFTAG_TILELENGTH, &tileh) ||
        tilew == 0 || tileh == 0)
        return;

    size_t npixels = (size_t)tilew * (size_t)tileh;
    if (npixels == 0 || npixels > SIZE_MAX / sizeof(uint32_t))
        return;

    uint32_t *raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
    if (!raster)
        return;

    uint32_t cols[4] = {0, tilew, tilew > 1 ? tilew - 1 : 0, 1};
    uint32_t rows[4] = {0, tileh, tileh > 1 ? tileh - 1 : 0, 1};
    int stops[2] = {0, 1};

    for (size_t ci = 0; ci < 4; ++ci)
    {
        for (size_t ri = 0; ri < 4; ++ri)
        {
            for (size_t si = 0; si < 2; ++si)
            {
                (void)TIFFReadRGBATileExt(tif, cols[ci], rows[ri], raster, stops[si]);
            }
        }
    }

    free(raster);
}

static int safe_for_user_buffer(TIFF *tif)
{
    uint32_t rowsperstrip = 0;
    uint32_t imagelength = 0;
    uint32_t tilewidth = 0;
    uint32_t tilelength = 0;

    if (get_u32_tag(tif, TIFFTAG_TILEWIDTH, &tilewidth) &&
        get_u32_tag(tif, TIFFTAG_TILELENGTH, &tilelength))
    {
        return tilewidth != 0 && tilelength != 0;
    }

    if (get_u32_tag(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip) &&
        get_u32_tag(tif, TIFFTAG_IMAGELENGTH, &imagelength))
    {
        (void)imagelength;
        return rowsperstrip != 0;
    }

    return 0;
}

static void fuzz_user_buffer(TIFF *tif, const uint8_t *Data, size_t Size)
{
    if (Size == 0 || !safe_for_user_buffer(tif))
        return;

    tmsize_t insize = (tmsize_t)Size;
    if (insize <= 0)
        return;

    size_t alloc_size = Size * 4u + 64u;
    if (alloc_size < Size || alloc_size > (size_t)((tmsize_t)-1 > 0 ? (tmsize_t)-1 : SIZE_MAX))
        alloc_size = Size + 64u;
    if (alloc_size == 0)
        return;

    tmsize_t outsize = (tmsize_t)alloc_size;
    if (outsize <= 0)
        return;

    void *outbuf = malloc(alloc_size);
    if (!outbuf)
        return;

    uint32_t striles[] = {0, 1, (uint32_t)(Size & 0xFFFFu), 0xFFFFFFFFu};
    for (size_t i = 0; i < sizeof(striles) / sizeof(striles[0]); ++i)
    {
        (void)TIFFReadFromUserBuffer(tif, striles[i], (void *)Data, insize, outbuf, outsize);
    }

    free(outbuf);
}

static void fuzz_tiff_handle(TIFF *tif, const uint8_t *Data, size_t Size)
{
    fuzz_common_queries(tif);
    fuzz_rgba_tile(tif);
    fuzz_user_buffer(tif, Data, Size);
    fuzz_directories(tif);
    fuzz_rgba_tile(tif);
    fuzz_user_buffer(tif, Data, Size);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif)
    {
        fuzz_tiff_handle(tif, Data, Size);
        TIFFClose(tif);
    }

    TIFF *tif_header = TIFFOpen("./dummy_file", "rh");
    if (tif_header)
    {
        fuzz_tiff_handle(tif_header, Data, Size);
        TIFFClose(tif_header);
    }

    return 0;
}