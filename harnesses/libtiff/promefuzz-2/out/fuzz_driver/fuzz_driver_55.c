// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithTag at tif_dirinfo.c:732:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithTag at tif_dirinfo.c:732:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithName at tif_dirinfo.c:744:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithName at tif_dirinfo.c:744:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithName at tif_dirinfo.c:744:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithName at tif_dirinfo.c:744:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFieldWithName at tif_dirinfo.c:744:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldTag at tif_dirinfo.c:756:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

#ifndef TIFF_VARIABLE
#define TIFF_VARIABLE -1
#endif

#ifndef TIFF_VARIABLE2
#define TIFF_VARIABLE2 -3
#endif

#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM 65
#endif

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void build_name(char *out, size_t outsz, const uint8_t *data, size_t len) {
    size_t i;
    if (outsz == 0) return;
    for (i = 0; i + 1 < outsz && i < len; ++i) {
        uint8_t c = data[i];
        if (c < 32 || c > 126)
            c = (uint8_t)('A' + (c % 26));
        out[i] = (char)c;
    }
    out[i] = '\0';
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    const TIFFField *field = NULL;
    const TIFFField *field2 = NULL;
    char namebuf[64];
    uint32_t tag1 = 256;
    uint32_t tag2 = 257;
    uint32_t custom_tag = 65000;
    TIFFDataType dtype = TIFF_ANY;
    TIFFFieldInfo finfo[2];
    int merge_count = 1;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
        if (!tif)
            return 0;
    }

    if (Size >= 4) {
        tag1 = rd16(Data);
        tag2 = rd16(Data + 2);
    } else if (Size >= 2) {
        tag1 = rd16(Data);
        tag2 = tag1 ^ 0x1234U;
    }

    if (Size >= 8)
        custom_tag = 32768U + (rd32(Data + 4) % 32767U);
    else
        custom_tag = 65000U;

    if (Size > 0) {
        switch (Data[0] % 6) {
            case 0: dtype = TIFF_ANY; break;
            case 1: dtype = TIFF_BYTE; break;
            case 2: dtype = TIFF_ASCII; break;
            case 3: dtype = TIFF_SHORT; break;
            case 4: dtype = TIFF_LONG; break;
            default: dtype = TIFF_UNDEFINED; break;
        }
    }

    memset(finfo, 0, sizeof(finfo));
    finfo[0].field_tag = (ttag_t)custom_tag;
    finfo[0].field_readcount = TIFF_VARIABLE;
    finfo[0].field_writecount = TIFF_VARIABLE;
    finfo[0].field_type = dtype;
    finfo[0].field_bit = FIELD_CUSTOM;
    finfo[0].field_oktochange = 1;
    finfo[0].field_passcount = 1;
    finfo[0].field_name = (char *)"FuzzCustomTagA";

    finfo[1].field_tag = (ttag_t)(custom_tag + 1);
    finfo[1].field_readcount = 1;
    finfo[1].field_writecount = 1;
    finfo[1].field_type = TIFF_LONG;
    finfo[1].field_bit = FIELD_CUSTOM;
    finfo[1].field_oktochange = 1;
    finfo[1].field_passcount = 0;
    finfo[1].field_name = (char *)"FuzzCustomTagB";

    if (Size > 1)
        merge_count = (Data[1] & 1) ? 2 : 1;

    (void)TIFFMergeFieldInfo(tif, finfo, (uint32_t)merge_count);

    field = TIFFFindField(tif, tag1, dtype);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFindField(tif, tag1, TIFF_ANY);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFieldWithTag(tif, tag2);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFieldWithTag(tif, custom_tag);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFieldWithName(tif, "ImageWidth");
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFieldWithName(tif, "ImageLength");
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFieldWithName(tif, "FuzzCustomTagA");
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    if (merge_count > 1) {
        field = TIFFFieldWithName(tif, "FuzzCustomTagB");
        if (field) {
            (void)TIFFFieldTag(field);
            (void)TIFFFieldName(field);
        }
    }

    if (Size > 8) {
        build_name(namebuf, sizeof(namebuf), Data + 8, Size - 8);
        field2 = TIFFFieldWithName(tif, namebuf);
        if (field2) {
            (void)TIFFFieldTag(field2);
            (void)TIFFFieldName(field2);
        }
    }

    field = TIFFFindField(tif, custom_tag, dtype);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    field = TIFFFindField(tif, custom_tag + 1, TIFF_LONG);
    if (field) {
        (void)TIFFFieldTag(field);
        (void)TIFFFieldName(field);
    }

    TIFFClose(tif);
    return 0;
}