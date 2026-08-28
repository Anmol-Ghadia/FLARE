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

static void exercise_field_accessors(const TIFFField *field) {
    if (!field) return;
    (void)TIFFFieldReadCount(field);
    (void)TIFFFieldWriteCount(field);
    (void)TIFFFieldDataType(field);
    (void)TIFFFieldTag(field);
    (void)TIFFFieldPassCount(field);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "rm");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "rC");
    }
    if (!tif) {
        return 0;
    }

    uint32_t tags_to_try[16];
    size_t n = 0;

    tags_to_try[n++] = 0;
    tags_to_try[n++] = 1;
    tags_to_try[n++] = 255;
    tags_to_try[n++] = 256;   /* ImageWidth */
    tags_to_try[n++] = 257;   /* ImageLength */
    tags_to_try[n++] = 258;   /* BitsPerSample */
    tags_to_try[n++] = 259;   /* Compression */
    tags_to_try[n++] = 262;   /* PhotometricInterpretation */
    tags_to_try[n++] = 273;   /* StripOffsets */
    tags_to_try[n++] = 277;   /* SamplesPerPixel */
    tags_to_try[n++] = 278;   /* RowsPerStrip */
    tags_to_try[n++] = 279;   /* StripByteCounts */
    tags_to_try[n++] = 282;   /* XResolution */
    tags_to_try[n++] = 283;   /* YResolution */
    tags_to_try[n++] = 296;   /* ResolutionUnit */
    tags_to_try[n++] = 65535;

    if (Size >= 4) {
        uint32_t derived =
            ((uint32_t)Data[0]) |
            ((uint32_t)Data[1] << 8) |
            ((uint32_t)Data[2] << 16) |
            ((uint32_t)Data[3] << 24);
        tags_to_try[0] = derived;
    } else if (Size > 0) {
        tags_to_try[0] = Data[0];
    }

    for (size_t i = 0; i < n; ++i) {
        const TIFFField *field = TIFFFieldWithTag(tif, tags_to_try[i]);
        exercise_field_accessors(field);
    }

    if (Size > 4) {
        for (size_t i = 0; i + 1 < Size && i < 32; i += 2) {
            uint32_t tag = ((uint32_t)Data[i] << 8) | (uint32_t)Data[i + 1];
            const TIFFField *field = TIFFFieldWithTag(tif, tag);
            exercise_field_accessors(field);
        }
    }

    TIFFClose(tif);
    return 0;
}