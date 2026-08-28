#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

static int read_s16(const uint8_t *Data, size_t Size, size_t Off)
{
    return (int)(int16_t)read_u16(Data, Size, Off);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif = NULL;
    const TIFFField *field = NULL;
    uint32_t tag = 0;
    TIFFDataType dtype;
    TIFFDataType queried_type;
    int width1, width2, width3;
    int rc, wc, pc;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL)
    {
        if (Size >= 4)
            tag = read_u32(Data, Size, 0);
        else if (Size >= 2)
            tag = read_u16(Data, Size, 0);
        else if (Size >= 1)
            tag = Data[0];

        queried_type = (TIFFDataType)(Size > 4 ? Data[4] : 0);

        field = TIFFFindField(tif, tag, queried_type);
        if (field != NULL)
        {
            dtype = TIFFFieldDataType(field);
            width1 = TIFFDataWidth(dtype);
            rc = TIFFFieldReadCount(field);
            wc = TIFFFieldWriteCount(field);
            pc = TIFFFieldPassCount(field);

            (void)width1;
            (void)rc;
            (void)wc;
            (void)pc;

            width2 = TIFFDataWidth((TIFFDataType)0);
            width3 = TIFFDataWidth((TIFFDataType)(dtype + (TIFFDataType)(Size & 0xFF)));
            (void)width2;
            (void)width3;
        }

        for (i = 0; i < 8 && i < Size; i++)
        {
            uint32_t alt_tag = tag ^ ((uint32_t)Data[i] << ((i % 4) * 8));
            TIFFDataType alt_type = (TIFFDataType)Data[Size - 1 - i];
            const TIFFField *alt_field = TIFFFindField(tif, alt_tag, alt_type);
            if (alt_field != NULL)
            {
                TIFFDataType alt_dtype = TIFFFieldDataType(alt_field);
                int alt_width = TIFFDataWidth(alt_dtype);
                int alt_rc = TIFFFieldReadCount(alt_field);
                int alt_wc = TIFFFieldWriteCount(alt_field);
                int alt_pc = TIFFFieldPassCount(alt_field);
                (void)alt_width;
                (void)alt_rc;
                (void)alt_wc;
                (void)alt_pc;
            }
            else
            {
                (void)TIFFDataWidth(alt_type);
            }
        }

        TIFFClose(tif);
    }

    {
        TIFFField local_field;
        memset(&local_field, 0, sizeof(local_field));

        local_field.field_tag = read_u32(Data, Size, 0);
        local_field.field_readcount = (short)read_s16(Data, Size, 4);
        local_field.field_writecount = (short)read_s16(Data, Size, 6);
        local_field.field_type = (TIFFDataType)(Size > 8 ? Data[8] : 0);
        local_field.field_passcount = (unsigned char)(Size > 9 ? Data[9] : 0);

        dtype = TIFFFieldDataType(&local_field);
        rc = TIFFFieldReadCount(&local_field);
        wc = TIFFFieldWriteCount(&local_field);
        pc = TIFFFieldPassCount(&local_field);
        width1 = TIFFDataWidth(dtype);
        width2 = TIFFDataWidth((TIFFDataType)(rc & 0xFF));
        width3 = TIFFDataWidth((TIFFDataType)(wc & 0xFF));

        (void)rc;
        (void)wc;
        (void)pc;
        (void)width1;
        (void)width2;
        (void)width3;
    }

    return 0;
}