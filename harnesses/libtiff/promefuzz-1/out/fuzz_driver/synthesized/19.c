// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFGetConfiguredCODECs at tif_compress.c:251:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFRegisterCODEC at tif_compress.c:200:1 in tiffio.h
// TIFFRegisterCODEC at tif_compress.c:200:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFGetConfiguredCODECs at tif_compress.c:251:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFGetConfiguredCODECs at tif_compress.c:251:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
// TIFFSetTagExtender at tif_dir.c:1350:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
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

static TIFFExtendProc g_prev_extender = NULL;

static void DummyTagExtender(TIFF *tif)
{
    (void)tif;
    if (g_prev_extender)
        g_prev_extender(tif);
}

static int DummyCodecInit(TIFF *tif, int scheme)
{
    (void)tif;
    (void)scheme;
    return 1;
}

int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size)
{
    if (Data == NULL)
        return 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint16_t scheme1 = 0;
    uint16_t scheme2 = 0;
    uint16_t query1 = 0;
    uint16_t query2 = 0;
    uint16_t query3 = 0;

    if (Size >= 2)
        scheme1 = (uint16_t)((Data[0] << 8) | Data[1]);
    if (Size >= 4)
        scheme2 = (uint16_t)((Data[2] << 8) | Data[3]);
    else
        scheme2 = (uint16_t)(scheme1 ^ 0x00FF);
    if (Size >= 6)
        query1 = (uint16_t)((Data[4] << 8) | Data[5]);
    else
        query1 = scheme1;
    if (Size >= 8)
        query2 = (uint16_t)((Data[6] << 8) | Data[7]);
    else
        query2 = scheme2;
    if (Size >= 10)
        query3 = (uint16_t)((Data[8] << 8) | Data[9]);
    else
        query3 = (uint16_t)(scheme1 ^ scheme2);

    char name1[64];
    char name2[64];
    size_t off = 10;
    size_t rem = (Size > off) ? (Size - off) : 0;
    size_t n1 = rem > 24 ? 24 : rem;
    size_t n2 = rem > 48 ? 24 : (rem > 24 ? rem - 24 : 0);

    memset(name1, 0, sizeof(name1));
    memset(name2, 0, sizeof(name2));
    memcpy(name1, "fuzz_codec_1_", 13);
    memcpy(name2, "fuzz_codec_2_", 13);

    for (size_t i = 0; i < n1 && 13 + i < sizeof(name1) - 1; i++)
    {
        unsigned char c = Data[off + i];
        name1[13 + i] = (char)('A' + (c % 26));
    }
    for (size_t i = 0; i < n2 && 13 + i < sizeof(name2) - 1; i++)
    {
        unsigned char c = Data[off + 24 + i];
        name2[13 + i] = (char)('a' + (c % 26));
    }

    g_prev_extender = TIFFSetTagExtender(DummyTagExtender);
    TIFFExtendProc prev2 = TIFFSetTagExtender(NULL);
    (void)prev2;
    TIFFExtendProc prev3 = TIFFSetTagExtender(DummyTagExtender);
    (void)prev3;

    (void)TIFFFindCODEC(query1);
    (void)TIFFFindCODEC(query2);
    (void)TIFFFindCODEC(query3);

    (void)TIFFIsCODECConfigured(query1);
    (void)TIFFIsCODECConfigured(query2);
    (void)TIFFIsCODECConfigured(query3);

    TIFFCodec *list = TIFFGetConfiguredCODECs();
    if (list)
        _TIFFfree(list);

    TIFFCodec *registered1 = TIFFRegisterCODEC(scheme1, name1, DummyCodecInit);
    TIFFCodec *registered2 = TIFFRegisterCODEC(scheme2, name2, DummyCodecInit);

    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);
    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);

    TIFFCodec *list2 = TIFFGetConfiguredCODECs();
    if (list2)
        _TIFFfree(list2);

    if (Size > 0)
    {
        switch (Data[0] % 4)
        {
            case 0:
                if (registered1)
                    TIFFUnRegisterCODEC(registered1);
                if (registered2)
                    TIFFUnRegisterCODEC(registered2);
                break;
            case 1:
                if (registered2)
                    TIFFUnRegisterCODEC(registered2);
                if (registered1)
                    TIFFUnRegisterCODEC(registered1);
                break;
            case 2:
                if (registered1)
                {
                    TIFFUnRegisterCODEC(registered1);
                    registered1 = NULL;
                }
                if (registered1)
                    TIFFUnRegisterCODEC(registered1);
                if (registered2)
                    TIFFUnRegisterCODEC(registered2);
                break;
            default:
                if (registered2)
                {
                    TIFFUnRegisterCODEC(registered2);
                    registered2 = NULL;
                }
                if (registered1)
                    TIFFUnRegisterCODEC(registered1);
                break;
        }
    }
    else
    {
        if (registered1)
            TIFFUnRegisterCODEC(registered1);
        if (registered2)
            TIFFUnRegisterCODEC(registered2);
    }

    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);
    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);

    TIFFCodec *list3 = TIFFGetConfiguredCODECs();
    if (list3)
        _TIFFfree(list3);

    TIFFSetTagExtender(g_prev_extender);
    return 0;
}