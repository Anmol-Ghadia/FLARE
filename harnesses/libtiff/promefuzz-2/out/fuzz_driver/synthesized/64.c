// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFUnsetField at tif_dir.c:843:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFGetTagListCount at tif_extension.c:34:5 in tiffio.h
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

static uint16_t read_u16(const uint8_t *data, size_t size, size_t off)
{
    if (off + 1 >= size)
        return 0;
    return (uint16_t)(data[off] | ((uint16_t)data[off + 1] << 8));
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t off)
{
    if (off + 3 >= size)
        return 0;
    return (uint32_t)data[off] |
           ((uint32_t)data[off + 1] << 8) |
           ((uint32_t)data[off + 2] << 16) |
           ((uint32_t)data[off + 3] << 24);
}

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput_64(const uint8_t *Data, size_t Size)
{
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w+");
    if (!tif)
        return 0;

    (void)TIFFCreateDirectory(tif);

    uint32_t base_tag = 65000;
    if (Size >= 4)
        base_tag = 65000 + (read_u32(Data, Size, 0) % 500);

    char namebuf1[32];
    char namebuf2[32];
    char namebuf3[32];
    snprintf(namebuf1, sizeof(namebuf1), "fuzztag_%u_a", base_tag);
    snprintf(namebuf2, sizeof(namebuf2), "fuzztag_%u_b", base_tag + 1);
    snprintf(namebuf3, sizeof(namebuf3), "fuzztag_%u_c", base_tag + 2);

    TIFFFieldInfo infos[3];
    memset(infos, 0, sizeof(infos));

    infos[0].field_tag = base_tag;
    infos[0].field_readcount = TIFF_VARIABLE2;
    infos[0].field_writecount = TIFF_VARIABLE2;
    infos[0].field_type = TIFF_ASCII;
    infos[0].field_bit = FIELD_CUSTOM;
    infos[0].field_oktochange = 1;
    infos[0].field_passcount = 1;
    infos[0].field_name = namebuf1;

    infos[1].field_tag = base_tag + 1;
    infos[1].field_readcount = 1;
    infos[1].field_writecount = 1;
    infos[1].field_type = TIFF_LONG;
    infos[1].field_bit = FIELD_CUSTOM;
    infos[1].field_oktochange = 1;
    infos[1].field_passcount = 0;
    infos[1].field_name = namebuf2;

    infos[2].field_tag = base_tag + 2;
    infos[2].field_readcount = TIFF_VARIABLE;
    infos[2].field_writecount = TIFF_VARIABLE;
    infos[2].field_type = TIFF_SHORT;
    infos[2].field_bit = FIELD_CUSTOM;
    infos[2].field_oktochange = 1;
    infos[2].field_passcount = 1;
    infos[2].field_name = namebuf3;

    uint32_t merge_count = 1;
    if (Size > 4)
        merge_count = (read_u16(Data, Size, 4) % 3) + 1;
    (void)TIFFMergeFieldInfo(tif, infos, merge_count);

    (void)TIFFGetTagListCount(tif);

    for (size_t i = 0; i < 8; ++i)
    {
        uint32_t tag;
        if (Size >= (i + 1) * 4)
            tag = read_u32(Data, Size, i * 4);
        else
            tag = base_tag + (uint32_t)i;

        switch (i % 4)
        {
            case 0:
                tag = base_tag + (tag % 3);
                break;
            case 1:
                tag = tag & 0xFFFFu;
                break;
            case 2:
                tag = 0;
                break;
            default:
                tag = 65535u - (tag % 16u);
                break;
        }

        (void)TIFFUnsetField(tif, tag);
        (void)TIFFGetTagListCount(tif);

        if (Size > 0 && ((Data[i % Size] & 1u) != 0))
        {
            (void)TIFFCheckpointDirectory(tif);
        }

        if (Size > 0 && ((Data[i % Size] & 2u) != 0))
        {
            TIFFFreeDirectory(tif);
            (void)TIFFCreateDirectory(tif);
            (void)TIFFMergeFieldInfo(tif, infos, merge_count);
        }
    }

    (void)TIFFCheckpointDirectory(tif);
    TIFFFreeDirectory(tif);
    TIFFClose(tif);
    return 0;
}