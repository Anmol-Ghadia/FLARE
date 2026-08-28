#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size > 0) {
        fwrite(Data, 1, Size, fp);
    } else {
        static const unsigned char minimal_tiff[] = {
            'I', 'I', 42, 0, 8, 0, 0, 0, 0, 0
        };
        fwrite(minimal_tiff, 1, sizeof(minimal_tiff), fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        write_dummy_file(NULL, 0);
        tif = TIFFOpen("./dummy_file", "r");
        if (!tif) return 0;
    }

    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFIsBigEndian(tif);
    (void)TIFFGetMode(tif);
    (void)TIFFIsByteSwapped(tif);

    int original_mode = TIFFGetMode(tif);
    int new_mode = original_mode;
    if (Size >= sizeof(int)) {
        memcpy(&new_mode, Data, sizeof(int));
    } else if (Size > 0) {
        for (size_t i = 0; i < Size; ++i) {
            new_mode = (new_mode << 5) ^ Data[i];
        }
    } else {
        new_mode = 0;
    }

    int prev_mode = TIFFSetMode(tif, new_mode);
    (void)prev_mode;
    (void)TIFFGetMode(tif);
    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFIsBigEndian(tif);
    (void)TIFFIsByteSwapped(tif);

    int current_fd = open("./dummy_file", O_RDONLY);
    if (current_fd >= 0) {
        int old_fd = TIFFSetFileno(tif, current_fd);
        (void)old_fd;
        (void)TIFFIsMSB2LSB(tif);
        (void)TIFFIsBigEndian(tif);
        (void)TIFFGetMode(tif);
        (void)TIFFIsByteSwapped(tif);

        if (Size > 0) {
            int fuzz_fd = (int)Data[0];
            (void)TIFFSetFileno(tif, fuzz_fd);
            (void)TIFFIsMSB2LSB(tif);
            (void)TIFFIsBigEndian(tif);
            (void)TIFFGetMode(tif);
            (void)TIFFIsByteSwapped(tif);
            (void)TIFFSetFileno(tif, current_fd);
        }
    }

    (void)TIFFSetMode(tif, original_mode);
    (void)TIFFGetMode(tif);

    TIFFClose(tif);

    if (current_fd >= 0) {
        close(current_fd);
    }

    return 0;
}