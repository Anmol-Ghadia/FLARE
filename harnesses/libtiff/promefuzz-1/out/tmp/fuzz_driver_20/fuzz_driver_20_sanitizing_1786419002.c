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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t off) {
    uint32_t v = 0;
    if (off < Size) v |= (uint32_t)Data[off];
    if (off + 1 < Size) v |= ((uint32_t)Data[off + 1]) << 8;
    if (off + 2 < Size) v |= ((uint32_t)Data[off + 2]) << 16;
    if (off + 3 < Size) v |= ((uint32_t)Data[off + 3]) << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    static const uint32_t known_tags[] = {
        TIFFTAG_IMAGEWIDTH,
        TIFFTAG_IMAGELENGTH,
        TIFFTAG_BITSPERSAMPLE,
        TIFFTAG_COMPRESSION,
        TIFFTAG_PHOTOMETRIC,
        TIFFTAG_FILLORDER,
        TIFFTAG_DOCUMENTNAME,
        TIFFTAG_IMAGEDESCRIPTION,
        TIFFTAG_MAKE,
        TIFFTAG_MODEL,
        TIFFTAG_STRIPOFFSETS,
        TIFFTAG_ORIENTATION,
        TIFFTAG_SAMPLESPERPIXEL,
        TIFFTAG_ROWSPERSTRIP,
        TIFFTAG_STRIPBYTECOUNTS,
        TIFFTAG_XRESOLUTION,
        TIFFTAG_YRESOLUTION,
        TIFFTAG_PLANARCONFIG,
        TIFFTAG_RESOLUTIONUNIT,
        TIFFTAG_SOFTWARE,
        TIFFTAG_DATETIME,
        TIFFTAG_ARTIST,
        TIFFTAG_HOSTCOMPUTER,
        TIFFTAG_TILEWIDTH,
        TIFFTAG_TILELENGTH,
        TIFFTAG_TILEOFFSETS,
        TIFFTAG_TILEBYTECOUNTS,
        TIFFTAG_SUBIFD,
        0
    };
    static const char *known_names[] = {
        "ImageWidth",
        "ImageLength",
        "BitsPerSample",
        "Compression",
        "PhotometricInterpretation",
        "FillOrder",
        "DocumentName",
        "ImageDescription",
        "Make",
        "Model",
        "StripOffsets",
        "Orientation",
        "SamplesPerPixel",
        "RowsPerStrip",
        "StripByteCounts",
        "XResolution",
        "YResolution",
        "PlanarConfiguration",
        "ResolutionUnit",
        "Software",
        "DateTime",
        "Artist",
        "HostComputer",
        "TileWidth",
        "TileLength",
        "TileOffsets",
        "TileByteCounts",
        "SubIFD",
        "",
        NULL
    };

    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "rm");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r");
    }
    if (!tif) {
        return 0;
    }

    uint32_t fuzz_tag = read_u32(Data, Size, 0);
    TIFFDataType fuzz_type = (TIFFDataType)(Size > 4 ? (Data[4] % 32) : 0);

    const TIFFField *field = NULL;
    const TIFFField *field2 = NULL;
    const TIFFField *field3 = NULL;

    field = TIFFFindField(tif, fuzz_tag, fuzz_type);
    if (field) {
        (void)TIFFFieldDataType(field);
        (void)TIFFFieldReadCount(field);
        (void)TIFFFieldTag(field);
    }

    field2 = TIFFFieldWithTag(tif, fuzz_tag);
    if (field2) {
        (void)TIFFFieldDataType(field2);
        (void)TIFFFieldReadCount(field2);
        (void)TIFFFieldTag(field2);
    }

    if (Size > 5) {
        size_t idx = Data[5] % (sizeof(known_names) / sizeof(known_names[0]) - 1);
        field3 = TIFFFieldWithName(tif, known_names[idx]);
        if (field3) {
            (void)TIFFFieldDataType(field3);
            (void)TIFFFieldReadCount(field3);
            (void)TIFFFieldTag(field3);
        }
    }

    for (size_t i = 0; i < sizeof(known_tags) / sizeof(known_tags[0]); i++) {
        uint32_t tag = known_tags[i];
        const TIFFField *f = TIFFFindField(tif, tag, TIFF_ANY);
        if (f) {
            (void)TIFFFieldDataType(f);
            (void)TIFFFieldReadCount(f);
            (void)TIFFFieldTag(f);
        }

        f = TIFFFieldWithTag(tif, tag);
        if (f) {
            (void)TIFFFieldDataType(f);
            (void)TIFFFieldReadCount(f);
            (void)TIFFFieldTag(f);
        }
    }

    for (size_t i = 0; known_names[i] != NULL; i++) {
        const TIFFField *f = TIFFFieldWithName(tif, known_names[i]);
        if (f) {
            (void)TIFFFieldDataType(f);
            (void)TIFFFieldReadCount(f);
            (void)TIFFFieldTag(f);
        }
    }

    TIFFClose(tif);
    return 0;
}