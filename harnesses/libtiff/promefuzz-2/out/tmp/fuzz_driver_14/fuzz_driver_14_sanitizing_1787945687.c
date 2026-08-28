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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
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

    const char *name1 = "fuzz_codec_1";
    const char *name2 = "fuzz_codec_2";

    TIFFCodec *reg1 = TIFFRegisterCODEC(scheme1, name1, dummy_init);
    TIFFCodec *reg2 = TIFFRegisterCODEC(scheme2, name2, dummy_init);

    (void)TIFFFindCODEC(scheme1);
    (void)TIFFFindCODEC(scheme2);
    (void)TIFFIsCODECConfigured(scheme1);
    (void)TIFFIsCODECConfigured(scheme2);

    configured = TIFFGetConfiguredCODECs();
    if (configured != NULL)
        _TIFFfree(configured);

    if (tif != NULL) {
        char *field_name = (char *)_TIFFmalloc(32);
        if (field_name != NULL) {
            memset(field_name, 0, 32);
            if (Size > 4) {
                size_t copy_len = Size - 4;
                if (copy_len > 31)
                    copy_len = 31;
                memcpy(field_name, Data + 4, copy_len);
            } else {
                memcpy(field_name, "fuzz_tag", 9);
            }

            TIFFFieldInfo finfo;
            memset(&finfo, 0, sizeof(finfo));
            finfo.field_tag = (Size >= 6) ? read_u16(Data + (Size >= 6 ? 4 : 0)) : 65000u;
            finfo.field_readcount = -1;
            finfo.field_writecount = -1;
            finfo.field_type = (TIFFDataType)((Size > 6) ? (Data[6] % 18) : TIFF_ASCII);
            finfo.reserved = 0;
            finfo.field_bit = 0;
            finfo.field_oktochange = 1;
            finfo.field_passcount = (unsigned char)((Size > 7) ? (Data[7] & 1) : 0);
            finfo.field_name = field_name;

            (void)TIFFMergeFieldInfo(tif, &finfo, 1);

            TIFFFieldInfo infos[2];
            memset(infos, 0, sizeof(infos));

            infos[0] = finfo;
            infos[1] = finfo;
            infos[1].field_tag = (uint32_t)(finfo.field_tag + 1);
            infos[1].field_name = field_name;

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