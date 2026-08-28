#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    write_dummy_file(Data, Size);

    uint64_t written_off1 = 0;
    uint64_t written_off2 = 0;
    toff_t read_off1 = 0;
    toff_t read_off2 = 0;
    toff_t read_off3 = 8;

    if (Size >= sizeof(toff_t))
        memcpy(&read_off1, Data, sizeof(toff_t));
    if (Size >= 2 * sizeof(toff_t))
        memcpy(&read_off2, Data + sizeof(toff_t), sizeof(toff_t));
    if (Size >= 3 * sizeof(toff_t))
        memcpy(&read_off3, Data + 2 * sizeof(toff_t), sizeof(toff_t));

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    if (tifw)
    {
        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off1);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off2);
        TIFFFreeDirectory(tifw);

        if (Size > 0)
        {
            if ((Data[0] & 1) == 0)
                (void)TIFFCreateEXIFDirectory(tifw);
            else
                (void)TIFFCreateGPSDirectory(tifw);

            (void)TIFFWriteCustomDirectory(tifw, &written_off2);
            TIFFFreeDirectory(tifw);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr)
    {
        const TIFFFieldArray *exif_info = _TIFFGetExifFields();
        const TIFFFieldArray *gps_info = _TIFFGetGpsFields();

        if (exif_info)
        {
            (void)TIFFReadCustomDirectory(tifr, read_off1, exif_info);
            TIFFFreeDirectory(tifr);

            (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off1, exif_info);
            TIFFFreeDirectory(tifr);
        }

        if (gps_info)
        {
            (void)TIFFReadCustomDirectory(tifr, read_off2, gps_info);
            TIFFFreeDirectory(tifr);

            (void)TIFFReadCustomDirectory(tifr, read_off3, gps_info);
            TIFFFreeDirectory(tifr);

            if (written_off2 != 0)
            {
                (void)TIFFReadCustomDirectory(tifr, (toff_t)written_off2, gps_info);
                TIFFFreeDirectory(tifr);
            }
        }

        if (Size > 0)
        {
            if ((Data[0] & 1) == 0)
                (void)TIFFCreateEXIFDirectory(tifr);
            else
                (void)TIFFCreateGPSDirectory(tifr);

            TIFFFreeDirectory(tifr);
        }

        TIFFClose(tifr);
    }

    return 0;
}