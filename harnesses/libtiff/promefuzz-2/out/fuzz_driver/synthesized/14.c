// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFUnRegisterCODEC at tif_compress.c:223:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFGetConfiguredCODECs at tif_compress.c:251:1 in tiffio.h
// _TIFFfree at tif_unix.c:326:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFIsCODECConfigured at tif_codec.c:149:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
// TIFFFindCODEC at tif_compress.c:185:1 in tiffio.h
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
// _TIFFmalloc at tif_unix.c:309:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static int dummy_init(TIFF *tif, int scheme)
{
    (void)tif;
    (void)scheme;
    return 1;
}

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
            (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif == NULL)
        tif = TIFFOpen("./dummy_file", "w");

    uint16_t scheme1 = 0;
    uint16_t scheme2 = 1;
    if (Size >= 2)
        scheme1 = read_u16(Data);
    if (Size >= 4)
        scheme2 = read_u16(Data + 2);
    if (scheme1 == scheme2)
        scheme2 ^= 0x00FFu;

    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);
    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);

    TIFFCodec *configured = TIFFGetConfiguredCODECs();
    if (configured != NULL)
        _TIFFfree(configured);

    TIFFCodec *reg1 = TIFFRegisterCODEC(scheme1, "fuzz_codec_1", dummy_init);
    TIFFCodec *reg2 = TIFFRegisterCODEC(scheme2, "fuzz_codec_2", dummy_init);

    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);
    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);

    configured = TIFFGetConfiguredCODECs();
    if (configured != NULL)
        _TIFFfree(configured);

    if (tif != NULL)
    {
        char *field_name = (char *)_TIFFmalloc(32);
        if (field_name != NULL)
        {
            memset(field_name, 0, 32);
            if (Size > 4)
            {
                size_t copy_len = Size - 4;
                if (copy_len > 31)
                    copy_len = 31;
                memcpy(field_name, Data + 4, copy_len);
            }
            else
            {
                memcpy(field_name, "fuzz_tag", 9);
            }

            uint32_t tag = 65000u;
            if (Size >= 6)
                tag = read_u16(Data + 4);

            TIFFDataType dtype = TIFF_ASCII;
            if (Size > 6)
                dtype = (TIFFDataType)(Data[6] % 18);

            TIFFFieldInfo finfo = {
                (ttag_t)tag,
                (short)-1,
                (short)-1,
                dtype,
                (unsigned short)0,
                (unsigned char)1,
                (unsigned char)((Size > 7) ? (Data[7] & 1) : 0),
                field_name
            };

            (void)TIFFMergeFieldInfo(tif, &finfo, 1);

            TIFFFieldInfo infos[2];
            infos[0] = finfo;
            infos[1] = finfo;
            infos[1].field_tag = (ttag_t)(tag + 1);

            (void)TIFFMergeFieldInfo(tif, infos, 2);

            _TIFFfree(field_name);
        }
    }

    if (reg1 != NULL)
        TIFFUnRegisterCODEC(reg1);
    if (reg2 != NULL && reg2 != reg1)
        TIFFUnRegisterCODEC(reg2);

    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);
    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);

    configured = TIFFGetConfiguredCODECs();
    if (configured != NULL)
        _TIFFfree(configured);

    if (tif != NULL)
        TIFFClose(tif);

    return 0;
}