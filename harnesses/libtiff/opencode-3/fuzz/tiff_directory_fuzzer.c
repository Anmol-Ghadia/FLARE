#include <stddef.h>
#include <stdint.h>

#include "tiff_fuzzing.h"

#define TIFF_FUZZ_MAX_MAIN_DIRECTORIES 32U
#define TIFF_FUZZ_MAX_SUBIFDS 8U
#define TIFF_FUZZ_MAX_CUSTOM_IFDS 4U

typedef struct
{
    uint64_t values[TIFF_FUZZ_MAX_SUBIFDS];
    size_t count;
} TIFFFuzzOffsetList;

static void TIFFDirectoryFuzzRecordOffset(TIFFFuzzOffsetList *list,
                                          uint64_t offset)
{
    if (list == NULL || offset == 0 || list->count >= TIFF_FUZZ_MAX_SUBIFDS)
        return;

    list->values[list->count++] = offset;
}

static void TIFFDirectoryFuzzTouchTagList(TIFF *tif)
{
    int tag_count;
    int tag_index;

    tag_count = TIFFGetTagListCount(tif);
    if (tag_count < 0)
        return;
    if (tag_count > 64)
        tag_count = 64;

    for (tag_index = 0; tag_index < tag_count; ++tag_index)
        (void)TIFFGetTagListEntry(tif, tag_index);
}

static void TIFFDirectoryFuzzQueryCurrentDirectory(TIFF *tif,
                                                   TIFFFuzzOffsetList *subifds,
                                                   TIFFFuzzOffsetList *exif_ifds,
                                                   TIFFFuzzOffsetList *gps_ifds)
{
    uint32_t image_width = 0;
    uint32_t image_length = 0;
    uint32_t rows_per_strip = 0;
    uint32_t tile_width = 0;
    uint32_t tile_length = 0;
    uint16_t bits_per_sample = 0;
    uint16_t samples_per_pixel = 0;
    uint16_t compression = 0;
    uint16_t photometric = 0;
    uint16_t planar_config = 0;
    uint16_t orientation = 0;
    uint16_t fill_order = 0;
    uint16_t predictor = 0;
    uint16_t subifd_count = 0;
    uint16_t extra_samples_count = 0;
    uint64_t exif_ifd = 0;
    uint64_t gps_ifd = 0;
    uint64_t *subifd_offsets = NULL;
    uint16_t *extra_samples = NULL;
    uint16_t *red_colormap = NULL;
    uint16_t *green_colormap = NULL;
    uint16_t *blue_colormap = NULL;
    uint32_t xml_packet_length = 0;
    void *xml_packet = NULL;
    uint16_t index;

    (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &image_width);
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &image_length);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &fill_order);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PREDICTOR, &predictor);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);

    if (TIFFIsTiled(tif))
    {
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_TILEWIDTH, &tile_width);
        (void)TIFFGetFieldDefaulted(tif, TIFFTAG_TILELENGTH, &tile_length);
    }

    (void)TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra_samples_count,
                       &extra_samples);
    (void)TIFFGetField(tif, TIFFTAG_COLORMAP, &red_colormap, &green_colormap,
                       &blue_colormap);
    (void)TIFFGetField(tif, TIFFTAG_XMLPACKET, &xml_packet_length, &xml_packet);

    if (TIFFGetField(tif, TIFFTAG_SUBIFD, &subifd_count, &subifd_offsets))
    {
        for (index = 0; index < subifd_count && index < TIFF_FUZZ_MAX_SUBIFDS;
             ++index)
            TIFFDirectoryFuzzRecordOffset(subifds, subifd_offsets[index]);
    }

    if (TIFFGetField(tif, TIFFTAG_EXIFIFD, &exif_ifd) &&
        exif_ifds->count < TIFF_FUZZ_MAX_CUSTOM_IFDS)
    {
        exif_ifds->values[exif_ifds->count++] = exif_ifd;
    }

    if (TIFFGetField(tif, TIFFTAG_GPSIFD, &gps_ifd) &&
        gps_ifds->count < TIFF_FUZZ_MAX_CUSTOM_IFDS)
    {
        gps_ifds->values[gps_ifds->count++] = gps_ifd;
    }

    TIFFDirectoryFuzzTouchTagList(tif);
}

static void TIFFDirectoryFuzzReadCustomDirectory(const uint8_t *data, size_t size,
                                                 uint64_t offset,
                                                 int (*reader)(TIFF *, toff_t))
{
    TIFFFuzzBuffer mem;
    TIFF *tif;

    if (offset == 0 || reader == NULL)
        return;

    tif = TIFFFuzzOpen(data, size, &mem);
    if (tif == NULL)
        return;

    if (reader(tif, (toff_t)offset))
        TIFFDirectoryFuzzTouchTagList(tif);

    TIFFClose(tif);
}

static void TIFFDirectoryFuzzReadSubDirectory(const uint8_t *data, size_t size,
                                              uint64_t offset)
{
    TIFFFuzzBuffer mem;
    TIFF *tif;
    TIFFFuzzOffsetList ignored_subifds = {{0}, 0};
    TIFFFuzzOffsetList ignored_exif_ifds = {{0}, 0};
    TIFFFuzzOffsetList ignored_gps_ifds = {{0}, 0};

    if (offset == 0)
        return;

    tif = TIFFFuzzOpen(data, size, &mem);
    if (tif == NULL)
        return;

    if (TIFFSetSubDirectory(tif, offset))
        TIFFDirectoryFuzzQueryCurrentDirectory(tif, &ignored_subifds,
                                               &ignored_exif_ifds,
                                               &ignored_gps_ifds);

    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    TIFFFuzzBuffer mem;
    TIFF *tif;
    TIFFFuzzOffsetList subifds = {{0}, 0};
    TIFFFuzzOffsetList exif_ifds = {{0}, 0};
    TIFFFuzzOffsetList gps_ifds = {{0}, 0};
    uint32_t directory;
    size_t index;

    tif = TIFFFuzzOpen(data, size, &mem);
    if (tif == NULL)
        return 0;

    for (directory = 0; directory < TIFF_FUZZ_MAX_MAIN_DIRECTORIES; ++directory)
    {
        TIFFDirectoryFuzzQueryCurrentDirectory(tif, &subifds, &exif_ifds,
                                               &gps_ifds);
        if (!TIFFReadDirectory(tif))
            break;
    }

    TIFFClose(tif);

    for (index = 0; index < exif_ifds.count; ++index)
    {
        TIFFDirectoryFuzzReadCustomDirectory(data, size, exif_ifds.values[index],
                                             TIFFReadEXIFDirectory);
    }

    for (index = 0; index < gps_ifds.count; ++index)
    {
        TIFFDirectoryFuzzReadCustomDirectory(data, size, gps_ifds.values[index],
                                             TIFFReadGPSDirectory);
    }

    for (index = 0; index < subifds.count; ++index)
        TIFFDirectoryFuzzReadSubDirectory(data, size, subifds.values[index]);

    return 0;
}
