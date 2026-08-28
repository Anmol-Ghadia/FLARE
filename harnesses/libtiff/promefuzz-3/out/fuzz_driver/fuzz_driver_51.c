// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFGetTagListEntry at tif_extension.c:42:10 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFReadCustomDirectory at tif_dirread.c:4441:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFGetTagListEntry at tif_extension.c:42:10 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

#ifndef TIFF_VARIABLE2
#define TIFF_VARIABLE2 (-3)
#endif

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
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
        return 0;
    }

    uint32_t baseTag = 65000;
    if (Size >= 2) {
        baseTag = 65000u + (rd16(Data, Size, 0) % 500u);
    }

    TIFFFieldInfo finfo[3];
    memset(finfo, 0, sizeof(finfo));

    finfo[0].field_tag = baseTag;
    finfo[0].field_readcount = 1;
    finfo[0].field_writecount = 1;
    finfo[0].field_type = TIFF_LONG;
    finfo[0].field_bit = FIELD_CUSTOM;
    finfo[0].field_oktochange = 1;
    finfo[0].field_passcount = 0;
    finfo[0].field_name = (char *)"FuzzCustomLong";

    finfo[1].field_tag = baseTag + 1;
    finfo[1].field_readcount = TIFF_VARIABLE2;
    finfo[1].field_writecount = TIFF_VARIABLE2;
    finfo[1].field_type = TIFF_ASCII;
    finfo[1].field_bit = FIELD_CUSTOM;
    finfo[1].field_oktochange = 1;
    finfo[1].field_passcount = 1;
    finfo[1].field_name = (char *)"FuzzCustomASCII";

    finfo[2].field_tag = baseTag + 2;
    finfo[2].field_readcount = 1;
    finfo[2].field_writecount = 1;
    finfo[2].field_type = TIFF_SHORT;
    finfo[2].field_bit = FIELD_CUSTOM;
    finfo[2].field_oktochange = 0;
    finfo[2].field_passcount = 0;
    finfo[2].field_name = (char *)"FuzzCustomShort";

    (void)TIFFMergeFieldInfo(tif, finfo, 3);

    {
        int count_before = TIFFGetTagListCount(tif);
        if (count_before < 0) count_before = 0;
        for (int i = -1; i <= count_before + 1; ++i) {
            (void)TIFFGetTagListEntry(tif, i);
        }
    }

    (void)TIFFUnsetField(tif, baseTag);
    (void)TIFFUnsetField(tif, baseTag + 1);
    (void)TIFFUnsetField(tif, baseTag + 2);
    if (Size >= 6) {
        (void)TIFFUnsetField(tif, rd32(Data, Size, 2));
    }

    {
        const TIFFFieldArray *infoarray = NULL;
        toff_t off1 = 0, off2 = 0, off3 = 0;

        if (Size >= 8) off1 = (toff_t)rd32(Data, Size, 4);
        if (Size >= 12) off2 = (toff_t)rd32(Data, Size, 8);
        if (Size >= 16) off3 = (toff_t)rd32(Data, Size, 12);

        (void)TIFFReadCustomDirectory(tif, off1, infoarray);
        (void)TIFFGetTagListCount(tif);
        TIFFFreeDirectory(tif);

        (void)TIFFReadCustomDirectory(tif, off2, infoarray);
        {
            int count_after = TIFFGetTagListCount(tif);
            if (count_after < 0) count_after = 0;
            for (int i = 0; i < count_after + 2; ++i) {
                uint32_t tag = TIFFGetTagListEntry(tif, i);
                (void)TIFFUnsetField(tif, tag);
            }
        }
        TIFFFreeDirectory(tif);

        (void)TIFFReadCustomDirectory(tif, off3, infoarray);
        (void)TIFFGetTagListCount(tif);
        TIFFFreeDirectory(tif);
    }

    TIFFClose(tif);
    return 0;
}