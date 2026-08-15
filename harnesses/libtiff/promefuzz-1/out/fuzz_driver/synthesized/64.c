// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFLastDirectory at tif_dir.c:1725:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFLastDirectory at tif_dir.c:1725:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFLastDirectory at tif_dir.c:1725:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFLastDirectory at tif_dir.c:1725:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFLastDirectory at tif_dir.c:1725:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetSubDirectory at tif_dir.c:1700:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_64(const uint8_t *Data, size_t Size) {
    uint64_t subdir_off = 0;
    uint16_t dirn = 0;
    int tiles = 0;
    int mode_selector = 0;

    if (Size >= 8) {
        memcpy(&subdir_off, Data, 8);
    } else {
        for (size_t i = 0; i < Size; i++) {
            subdir_off = (subdir_off << 8) | Data[i];
        }
    }

    if (Size >= 10) {
        memcpy(&dirn, Data + 8, 2);
    } else if (Size > 8) {
        dirn = Data[8];
    }

    if (Size >= 11) {
        tiles = Data[10] & 1;
    }
    if (Size >= 12) {
        mode_selector = Data[11] % 4;
    }

    write_dummy_file(Data, Size);

    {
        TIFF *tif = TIFFOpen("./dummy_file", "r");
        if (tif) {
            (void)TIFFIsByteSwapped(tif);
            (void)TIFFLastDirectory(tif);

            (void)TIFFSetDirectory(tif, 0);
            (void)TIFFLastDirectory(tif);
            (void)TIFFIsByteSwapped(tif);

            (void)TIFFSetDirectory(tif, dirn);
            (void)TIFFSetSubDirectory(tif, subdir_off);

            if (Size >= 16) {
                uint16_t dirn2;
                uint64_t subdir_off2;
                memcpy(&dirn2, Data + Size - 2, 2);
                memcpy(&subdir_off2, Data + (Size >= 8 ? Size - 8 : 0), Size >= 8 ? 8 : 0);
                (void)TIFFSetDirectory(tif, dirn2);
                (void)TIFFSetSubDirectory(tif, subdir_off2);
            }

            (void)TIFFLastDirectory(tif);
            (void)TIFFIsByteSwapped(tif);
            TIFFClose(tif);
        }
    }

    {
        const char *mode = "w";
        if (mode_selector == 1) mode = "w8";
        else if (mode_selector == 2) mode = "a";
        else if (mode_selector == 3) mode = "a8";

        TIFF *tifw = TIFFOpen("./dummy_file", mode);
        if (tifw) {
            (void)TIFFCreateDirectory(tifw);
            (void)TIFFLastDirectory(tifw);
            (void)TIFFIsByteSwapped(tifw);

            (void)TIFFWriteCheck(tifw, tiles, "LLVMFuzzer");
            (void)TIFFWriteCheck(tifw, !tiles, "LLVMFuzzerAlt");

            (void)TIFFSetDirectory(tifw, dirn);
            (void)TIFFSetSubDirectory(tifw, subdir_off);

            (void)TIFFCreateDirectory(tifw);
            (void)TIFFWriteCheck(tifw, 0, "AfterCreateDirectory");
            (void)TIFFWriteCheck(tifw, 1, "AfterCreateDirectoryTiles");

            TIFFClose(tifw);
        }
    }

    {
        TIFF *tifrw = TIFFOpen("./dummy_file", "r+");
        if (tifrw) {
            (void)TIFFIsByteSwapped(tifrw);
            (void)TIFFLastDirectory(tifrw);
            (void)TIFFCreateDirectory(tifrw);
            (void)TIFFWriteCheck(tifrw, tiles, "ReadWriteMode");
            (void)TIFFSetDirectory(tifrw, dirn);
            (void)TIFFSetSubDirectory(tifrw, subdir_off);
            TIFFClose(tifrw);
        }
    }

    return 0;
}