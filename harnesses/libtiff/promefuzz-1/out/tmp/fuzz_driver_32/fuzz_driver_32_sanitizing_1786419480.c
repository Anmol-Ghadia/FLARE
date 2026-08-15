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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "rm");
    if (tif != NULL)
    {
        uint32_t tag = 0;
        TIFFDataType dtype = (TIFFDataType)0;

        if (Size >= 4)
        {
            tag = ((uint32_t)Data[0] << 24) |
                  ((uint32_t)Data[1] << 16) |
                  ((uint32_t)Data[2] << 8) |
                  (uint32_t)Data[3];
        }
        else if (Size > 0)
        {
            for (size_t i = 0; i < Size; i++)
                tag = (tag << 8) ^ Data[i];
        }

        if (Size >= 5)
            dtype = (TIFFDataType)Data[4];

        (void)TIFFDataWidth(dtype);
        (void)TIFFDataWidth((TIFFDataType)0);
        (void)TIFFDataWidth((TIFFDataType)1);
        (void)TIFFDataWidth((TIFFDataType)2);
        (void)TIFFDataWidth((TIFFDataType)3);
        (void)TIFFDataWidth((TIFFDataType)4);
        (void)TIFFDataWidth((TIFFDataType)5);
        (void)TIFFDataWidth((TIFFDataType)6);
        (void)TIFFDataWidth((TIFFDataType)7);
        (void)TIFFDataWidth((TIFFDataType)8);
        (void)TIFFDataWidth((TIFFDataType)9);
        (void)TIFFDataWidth((TIFFDataType)10);
        (void)TIFFDataWidth((TIFFDataType)11);
        (void)TIFFDataWidth((TIFFDataType)12);
        (void)TIFFDataWidth((TIFFDataType)13);
        (void)TIFFDataWidth((TIFFDataType)16);
        (void)TIFFDataWidth((TIFFDataType)17);
        (void)TIFFDataWidth((TIFFDataType)18);
        (void)TIFFDataWidth((TIFFDataType)255);

        const TIFFField *field = TIFFFindField(tif, tag, dtype);
        if (field != NULL)
        {
            (void)TIFFFieldDataType(field);
            (void)TIFFFieldPassCount(field);
            (void)TIFFFieldReadCount(field);
            (void)TIFFFieldWriteCount(field);
            (void)TIFFDataWidth(TIFFFieldDataType(field));
        }

        field = TIFFFindField(tif, tag, TIFF_ANY);
        if (field != NULL)
        {
            (void)TIFFFieldDataType(field);
            (void)TIFFFieldPassCount(field);
            (void)TIFFFieldReadCount(field);
            (void)TIFFFieldWriteCount(field);
            (void)TIFFDataWidth(TIFFFieldDataType(field));
        }

        if (Size >= 8)
        {
            uint32_t tag2 = ((uint32_t)Data[Size - 4] << 24) |
                            ((uint32_t)Data[Size - 3] << 16) |
                            ((uint32_t)Data[Size - 2] << 8) |
                            (uint32_t)Data[Size - 1];
            TIFFDataType dtype2 = (TIFFDataType)(Data[Size / 2]);

            field = TIFFFindField(tif, tag2, dtype2);
            if (field != NULL)
            {
                (void)TIFFFieldDataType(field);
                (void)TIFFFieldPassCount(field);
                (void)TIFFFieldReadCount(field);
                (void)TIFFFieldWriteCount(field);
                (void)TIFFDataWidth(TIFFFieldDataType(field));
            }

            field = TIFFFindField(tif, tag2, TIFF_ANY);
            if (field != NULL)
            {
                (void)TIFFFieldDataType(field);
                (void)TIFFFieldPassCount(field);
                (void)TIFFFieldReadCount(field);
                (void)TIFFFieldWriteCount(field);
                (void)TIFFDataWidth(TIFFFieldDataType(field));
            }
        }

        TIFFClose(tif);
    }
    else
    {
        TIFFDataType dtype = (TIFFDataType)(Size ? Data[0] : 0);
        (void)TIFFDataWidth(dtype);
        (void)TIFFDataWidth((TIFFDataType)0);
        (void)TIFFDataWidth((TIFFDataType)255);
    }

    return 0;
}