#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp)
        return;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tifw = TIFFOpen("./dummy_file", "w8");
    uint64_t written_off = 0;

    if (tifw) {
        (void)TIFFCreateGPSDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off);
        TIFFFreeDirectory(tifw);

        (void)TIFFCreateEXIFDirectory(tifw);
        (void)TIFFWriteCustomDirectory(tifw, &written_off);
        TIFFFreeDirectory(tifw);

        if (Size > 0) {
            if ((Data[0] & 1) == 0) {
                (void)TIFFCreateGPSDirectory(tifw);
            } else {
                (void)TIFFCreateEXIFDirectory(tifw);
            }
            (void)TIFFWriteCustomDirectory(tifw, &written_off);
            TIFFFreeDirectory(tifw);
        }

        TIFFClose(tifw);
    }

    TIFF *tifr = TIFFOpen("./dummy_file", "r");
    if (tifr) {
        toff_t off1 = 0;
        toff_t off2 = (toff_t)written_off;
        toff_t off3 = 8;

        if (Size >= sizeof(toff_t))
            memcpy(&off1, Data, sizeof(toff_t));
        if (Size >= 2 * sizeof(toff_t))
            memcpy(&off2, Data + sizeof(toff_t), sizeof(toff_t));
        if (Size >= 3 * sizeof(toff_t))
            memcpy(&off3, Data + 2 * sizeof(toff_t), sizeof(toff_t));

        (void)TIFFReadCustomDirectory(tifr, off1, TIFFGetTagListEntry(tifr, 0) ? NULL : NULL);

        (void)TIFFReadCustomDirectory(tifr, off2, TIFFGetExifFields());
        TIFFFreeDirectory(tifr);

        (void)TIFFReadCustomDirectory(tifr, off3, TIFFGetGPSFields());
        TIFFFreeDirectory(tifr);

        if (Size > 0) {
            if ((Data[0] & 1) == 0) {
                (void)TIFFCreateGPSDirectory(tifr);
            } else {
                (void)TIFFCreateEXIFDirectory(tifr);
            }
            TIFFFreeDirectory(tifr);
        }

        TIFFClose(tifr);
    }

    return 0;
}