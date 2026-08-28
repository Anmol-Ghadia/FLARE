// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldDataType at tif_dirinfo.c:768:1 in tiffio.h
// TIFFFieldReadCount at tif_dirinfo.c:780:1 in tiffio.h
// TIFFFieldWriteCount at tif_dirinfo.c:786:1 in tiffio.h
// TIFFFieldPassCount at tif_dirinfo.c:774:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFDataWidth at tif_dirinfo.c:536:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <tiffio.h>

static uint16_t read_u16(const uint8_t *Data, size_t Size, size_t Off)
{
    if (Off + 1 >= Size)
        return 0;
    return (uint16_t)(Data[Off] | ((uint16_t)Data[Off + 1] << 8));
}

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t Off)
{
    if (Off + 3 >= Size)
        return 0;
    return (uint32_t)Data[Off] |
           ((uint32_t)Data[Off + 1] << 8) |
           ((uint32_t)Data[Off + 2] << 16) |
           ((uint32_t)Data[Off + 3] << 24);
}

int LLVMFuzzerTestOneInput_65(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    TIFF *tif = NULL;
    uint32_t tag = 0;
    size_t i;

    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif == NULL)
    {
        for (i = 0; i < 16 && i < Size; ++i)
            (void)TIFFDataWidth((TIFFDataType)Data[i]);
        (void)TIFFDataWidth((TIFFDataType)0);
        return 0;
    }

    if (Size >= 4)
        tag = read_u32(Data, Size, 0);
    else if (Size >= 2)
        tag = read_u16(Data, Size, 0);
    else if (Size >= 1)
        tag = Data[0];

    for (i = 0; i < 16; ++i)
    {
        uint32_t alt_tag = tag;
        TIFFDataType qtype;
        const TIFFField *field;

        if (Size > i)
            alt_tag ^= ((uint32_t)Data[i] << ((i % 4) * 8));

        qtype = (TIFFDataType)((Size > (16 + i)) ? Data[16 + i] : i);
        field = TIFFFindField(tif, alt_tag, qtype);

        if (field != NULL)
        {
            TIFFDataType dtype = TIFFFieldDataType(field);
            int rc = TIFFFieldReadCount(field);
            int wc = TIFFFieldWriteCount(field);
            int pc = TIFFFieldPassCount(field);
            int w1 = TIFFDataWidth(dtype);
            int w2 = TIFFDataWidth(qtype);
            int w3 = TIFFDataWidth((TIFFDataType)(rc & 0xFF));
            int w4 = TIFFDataWidth((TIFFDataType)(wc & 0xFF));
            int w5 = TIFFDataWidth((TIFFDataType)(pc & 0xFF));
            (void)rc;
            (void)wc;
            (void)pc;
            (void)w1;
            (void)w2;
            (void)w3;
            (void)w4;
            (void)w5;
        }
        else
        {
            (void)TIFFDataWidth(qtype);
        }
    }

    TIFFClose(tif);
    return 0;
}