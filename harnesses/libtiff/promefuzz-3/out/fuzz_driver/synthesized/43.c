// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFIsMSB2LSB at tif_open.c:658:1 in tiffio.h
// TIFFIsBigEndian at tif_open.c:667:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFRGBAImageEnd at tif_getimage.c:215:1 in tiffio.h
// TIFFRGBAImageBegin at tif_getimage.c:266:1 in tiffio.h
// TIFFRGBAImageEnd at tif_getimage.c:215:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFIsByteSwapped at tif_open.c:640:1 in tiffio.h
// TIFFIsMSB2LSB at tif_open.c:658:1 in tiffio.h
// TIFFIsBigEndian at tif_open.c:667:1 in tiffio.h
// TIFFSetDirectory at tif_dir.c:1666:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

int LLVMFuzzerTestOneInput_43(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    TIFFRGBAImage rgba;
    char emsg[1024];
    int ok;
    uint16_t dirn = 0;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "rm");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "rC");
    }
    if (!tif) {
        return 0;
    }

    (void)TIFFIsByteSwapped(tif);
    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFIsBigEndian(tif);

    if (Size >= 2) {
        dirn = (uint16_t)((uint16_t)Data[0] | ((uint16_t)Data[1] << 8));
    } else if (Size == 1) {
        dirn = Data[0];
    }

    (void)TIFFSetDirectory(tif, 0);
    (void)TIFFSetDirectory(tif, 1);
    (void)TIFFSetDirectory(tif, dirn);
    (void)TIFFSetDirectory(tif, (uint16_t)(dirn ^ 0xFFFFu));

    memset(&rgba, 0, sizeof(rgba));
    memset(emsg, 0, sizeof(emsg));
    ok = TIFFRGBAImageBegin(&rgba, tif, 0, emsg);
    if (ok) {
        TIFFRGBAImageEnd(&rgba);
    }

    memset(&rgba, 0, sizeof(rgba));
    memset(emsg, 0, sizeof(emsg));
    ok = TIFFRGBAImageBegin(&rgba, tif, 1, emsg);
    if (ok) {
        TIFFRGBAImageEnd(&rgba);
    }

    (void)TIFFCreateDirectory(tif);
    (void)TIFFIsByteSwapped(tif);
    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFIsBigEndian(tif);
    (void)TIFFSetDirectory(tif, dirn);

    TIFFClose(tif);
    return 0;
}