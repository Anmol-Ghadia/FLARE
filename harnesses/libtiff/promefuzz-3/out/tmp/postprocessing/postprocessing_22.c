#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "w8");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        return 0;
    }

    TIFFTagMethods *methods = TIFFAccessTagMethods(tif);
    (void)methods;

    (void)TIFFCreateDirectory(tif);
    (void)TIFFAccessTagMethods(tif);

    (void)TIFFCreateEXIFDirectory(tif);
    (void)TIFFAccessTagMethods(tif);

    if (Size > 0) {
        size_t iterations = (Data[0] % 16) + 1;
        for (size_t i = 0; i < iterations; ++i) {
            uint8_t op = Data[(i + 1) % Size];
            switch (op % 4) {
                case 0:
                    (void)TIFFCreateDirectory(tif);
                    break;
                case 1:
                    (void)TIFFCreateEXIFDirectory(tif);
                    break;
                case 2:
                    (void)TIFFWriteDirectory(tif);
                    break;
                case 3:
                    TIFFFreeDirectory(tif);
                    break;
            }
            (void)TIFFAccessTagMethods(tif);
        }
    } else {
        (void)TIFFWriteDirectory(tif);
        TIFFFreeDirectory(tif);
        (void)TIFFCreateDirectory(tif);
        (void)TIFFCreateEXIFDirectory(tif);
    }

    TIFFClose(tif);
    return 0;
}