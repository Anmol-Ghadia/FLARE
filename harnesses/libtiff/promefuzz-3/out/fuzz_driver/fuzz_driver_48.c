// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFGetConfiguredCODECs at tif_compress.c:251:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFRegisterCODEC at tif_compress.c:200:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFRegisterCODEC at tif_compress.c:200:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFRGBAImageOK at tif_getimage.c:75:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

static int DummyInit(TIFF *tif, int scheme)
{
    (void)tif;
    (void)scheme;
    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint16_t scheme = 0;
    if (Size >= 2)
        scheme = (uint16_t)((uint16_t)Data[0] | ((uint16_t)Data[1] << 8));
    else if (Size == 1)
        scheme = Data[0];

    (void)TIFFIsCODECConfigured(scheme);
    (void)TIFFFindCODEC(scheme);

    TIFFCodec *configured = TIFFGetConfiguredCODECs();
    if (configured != NULL)
        _TIFFfree(configured);

    char namebuf[64];
    size_t name_len = 0;
    if (Size > 2)
    {
        name_len = Size - 2;
        if (name_len > sizeof(namebuf) - 1)
            name_len = sizeof(namebuf) - 1;
        memcpy(namebuf, Data + 2, name_len);
    }
    else
    {
        const char *fallback = "fuzzcodec";
        name_len = strlen(fallback);
        memcpy(namebuf, fallback, name_len);
    }
    namebuf[name_len] = '\0';

    TIFFCodec *registered = TIFFRegisterCODEC(scheme, namebuf, DummyInit);
    if (registered != NULL)
    {
        (void)TIFFFindCODEC(scheme);
        (void)TIFFIsCODECConfigured(scheme);
        TIFFUnRegisterCODEC(registered);
    }

    if (Size >= 4)
    {
        uint16_t scheme2 = (uint16_t)((uint16_t)Data[Size - 2] |
                                      ((uint16_t)Data[Size - 1] << 8));
        TIFFCodec *registered2 = TIFFRegisterCODEC(scheme2, "altcodec", DummyInit);
        if (registered2 != NULL)
            TIFFUnRegisterCODEC(registered2);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif != NULL)
    {
        char emsg[1024];
        memset(emsg, 0, sizeof(emsg));
        (void)TIFFRGBAImageOK(tif, emsg);
        TIFFClose(tif);
    }

    return 0;
}