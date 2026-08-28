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

static TIFFExtendProc g_prev_extender = NULL;

static void fuzz_extender(TIFF *tif)
{
    static const TIFFFieldInfo extra_fields[] = {
        {65000, -1, -1, TIFF_ASCII, FIELD_CUSTOM, 1, 0, (char *)"FuzzASCII"},
        {65001, 1, 1, TIFF_LONG, FIELD_CUSTOM, 1, 0, (char *)"FuzzLONG"},
        {65002, -1, -1, TIFF_BYTE, FIELD_CUSTOM, 0, 1, (char *)"FuzzBYTEPass"},
        {65003, 2, 2, TIFF_SHORT, FIELD_CUSTOM, 1, 0, (char *)"FuzzSHORT2"},
    };

    if (g_prev_extender)
        g_prev_extender(tif);

    if (tif)
        (void)TIFFMergeFieldInfo(tif, extra_fields,
                                 (uint32_t)(sizeof(extra_fields) / sizeof(extra_fields[0])));
}

static uint16_t read_u16(const uint8_t *p, size_t n)
{
    if (n < 2)
        return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p, size_t n)
{
    if (n < 4)
        return 0;
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif = NULL;
    const TIFFField *field = NULL;
    const char *name = NULL;
    char namebuf[128];
    size_t namelen = 0;
    uint32_t tag = 0;
    TIFFDataType dtype = TIFF_ANY;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    g_prev_extender = TIFFSetTagExtender(fuzz_extender);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        tif = TIFFOpen("./dummy_file", "rM");

    if (tif) {
        static const TIFFFieldInfo local_fields[] = {
            {65100, -1, -1, TIFF_ASCII, FIELD_CUSTOM, 1, 0, (char *)"LocalASCII"},
            {65101, 1, 1, TIFF_LONG, FIELD_CUSTOM, 1, 0, (char *)"LocalLONG"},
            {65102, 4, 4, TIFF_BYTE, FIELD_CUSTOM, 1, 0, (char *)"LocalBYTE4"},
            {65103, -1, -1, TIFF_UNDEFINED, FIELD_CUSTOM, 0, 1, (char *)"LocalUNDEF"},
        };
        (void)TIFFMergeFieldInfo(tif, local_fields,
                                 (uint32_t)(sizeof(local_fields) / sizeof(local_fields[0])));

        if (Size > 0) {
            namelen = Data[0] % (sizeof(namebuf) - 1);
            if (namelen > Size - 1)
                namelen = Size - 1;
            if (namelen > 0)
                memcpy(namebuf, Data + 1, namelen);
            namebuf[namelen] = '\0';
        } else {
            namebuf[0] = '\0';
        }

        if (Size >= 6) {
            tag = read_u32(Data + (Size >= 10 ? 2 : 0), Size - (Size >= 10 ? 2 : 0));
            switch (Data[1] % 8) {
                case 0: dtype = TIFF_ANY; break;
                case 1: dtype = TIFF_BYTE; break;
                case 2: dtype = TIFF_ASCII; break;
                case 3: dtype = TIFF_SHORT; break;
                case 4: dtype = TIFF_LONG; break;
                case 5: dtype = TIFF_RATIONAL; break;
                case 6: dtype = TIFF_UNDEFINED; break;
                default: dtype = TIFF_IFD; break;
            }
        } else {
            tag = read_u16(Data, Size);
            dtype = TIFF_ANY;
        }

        field = TIFFFieldWithName(tif, namebuf);
        if (field) {
            name = TIFFFieldName(field);
            (void)name;
        }

        field = TIFFFindField(tif, tag, dtype);
        if (field) {
            name = TIFFFieldName(field);
            (void)name;
        }

        field = TIFFFindField(tif, tag, TIFF_ANY);
        if (field) {
            name = TIFFFieldName(field);
            (void)name;
        }

        field = TIFFFieldWithTag(tif, tag);
        if (field) {
            name = TIFFFieldName(field);
            (void)name;
        }

        field = TIFFFieldWithName(tif, "FuzzASCII");
        if (field)
            (void)TIFFFieldName(field);

        field = TIFFFieldWithName(tif, "FuzzLONG");
        if (field)
            (void)TIFFFieldName(field);

        field = TIFFFieldWithName(tif, "LocalASCII");
        if (field)
            (void)TIFFFieldName(field);

        field = TIFFFieldWithTag(tif, 65000);
        if (field)
            (void)TIFFFieldName(field);

        field = TIFFFieldWithTag(tif, 65100);
        if (field)
            (void)TIFFFieldName(field);

        field = TIFFFindField(tif, 65001, TIFF_LONG);
        if (field)
            (void)TIFFFieldName(field);

        TIFFClose(tif);
    }

    (void)TIFFSetTagExtender(g_prev_extender);
    remove("./dummy_file");
    return 0;
}