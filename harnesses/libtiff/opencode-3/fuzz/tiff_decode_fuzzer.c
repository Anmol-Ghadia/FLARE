#include <stdint.h>

#include "tiff_fuzzing.h"

#define TIFF_FUZZ_MAX_ALLOCATION (128U * 1024U * 1024U)
#define TIFF_FUZZ_MAX_DIRECTORIES 8U
#define TIFF_FUZZ_MAX_SCANLINES 32U
#define TIFF_FUZZ_MAX_STRILES 32U
#define TIFF_FUZZ_MAX_SAMPLES 4U

static void TIFFDecodeFuzzReadScanlines(TIFF *tif)
{
    uint64_t scanline_size;
    uint32_t image_length = 0;
    uint16_t planar_config = PLANARCONFIG_CONTIG;
    uint16_t samples_per_pixel = 1;
    uint8_t *buffer;
    uint32_t row_limit;
    uint16_t sample_limit;
    uint16_t sample;
    uint32_t row;

    if (TIFFIsTiled(tif))
        return;

    scanline_size = TIFFScanlineSize64(tif);
    buffer = (uint8_t *)TIFFFuzzMalloc(scanline_size, TIFF_FUZZ_MAX_ALLOCATION);
    if (buffer == NULL)
        return;

    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &image_length);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);

    row_limit = image_length;
    if (row_limit > TIFF_FUZZ_MAX_SCANLINES)
        row_limit = TIFF_FUZZ_MAX_SCANLINES;

    sample_limit = planar_config == PLANARCONFIG_SEPARATE ? samples_per_pixel : 1;
    if (sample_limit > TIFF_FUZZ_MAX_SAMPLES)
        sample_limit = TIFF_FUZZ_MAX_SAMPLES;

    for (sample = 0; sample < sample_limit; ++sample)
    {
        for (row = 0; row < row_limit; ++row)
            (void)TIFFReadScanline(tif, buffer, row, sample);
    }

    _TIFFfree(buffer);
}

static void TIFFDecodeFuzzReadStrips(TIFF *tif)
{
    uint64_t strip_size;
    uint32_t strip_count;
    uint8_t *buffer;
    uint32_t strip;

    if (TIFFIsTiled(tif))
        return;

    strip_size = TIFFStripSize64(tif);
    buffer = (uint8_t *)TIFFFuzzMalloc(strip_size, TIFF_FUZZ_MAX_ALLOCATION);
    if (buffer == NULL)
        return;

    strip_count = TIFFNumberOfStrips(tif);
    if (strip_count > TIFF_FUZZ_MAX_STRILES)
        strip_count = TIFF_FUZZ_MAX_STRILES;

    for (strip = 0; strip < strip_count; ++strip)
        (void)TIFFReadEncodedStrip(tif, strip, buffer, (tmsize_t)strip_size);

    _TIFFfree(buffer);
}

static void TIFFDecodeFuzzReadTiles(TIFF *tif)
{
    uint64_t tile_size;
    uint32_t tile_count;
    uint8_t *buffer;
    uint32_t tile;

    if (!TIFFIsTiled(tif))
        return;

    tile_size = TIFFTileSize64(tif);
    buffer = (uint8_t *)TIFFFuzzMalloc(tile_size, TIFF_FUZZ_MAX_ALLOCATION);
    if (buffer == NULL)
        return;

    tile_count = TIFFNumberOfTiles(tif);
    if (tile_count > TIFF_FUZZ_MAX_STRILES)
        tile_count = TIFF_FUZZ_MAX_STRILES;

    for (tile = 0; tile < tile_count; ++tile)
        (void)TIFFReadEncodedTile(tif, tile, buffer, (tmsize_t)tile_size);

    _TIFFfree(buffer);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    TIFFFuzzBuffer mem;
    TIFF *tif;
    uint32_t directory;

    tif = TIFFFuzzOpen(data, size, &mem);
    if (tif == NULL)
        return 0;

    for (directory = 0; directory < TIFF_FUZZ_MAX_DIRECTORIES; ++directory)
    {
        TIFFDecodeFuzzReadScanlines(tif);
        TIFFDecodeFuzzReadStrips(tif);
        TIFFDecodeFuzzReadTiles(tif);

        if (!TIFFReadDirectory(tif))
            break;
    }

    TIFFClose(tif);
    return 0;
}
