#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif;
    TIFFTagMethods *methods;
    thandle_t client;
    uint32_t count = 0, i;
    size_t pos = 0;
    TIFFFieldInfo *infos = NULL;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    tif = TIFFOpen("./dummy_file", "r");
    if (tif == NULL) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (tif == NULL) {
        return 0;
    }

    methods = TIFFAccessTagMethods(tif);
    (void)methods;

    client = TIFFClientdata(tif);
    (void)client;

    (void)TIFFCreateDirectory(tif);

    if (Size >= 1) {
        count = (uint32_t)(Data[0] % 8);
        pos = 1;

        if (count > 0) {
            infos = (TIFFFieldInfo *)calloc((size_t)count, sizeof(TIFFFieldInfo));
            if (infos != NULL) {
                for (i = 0; i < count; i++) {
                    uint32_t tag = 65000U + i;
                    short readcount = 1;
                    short writecount = 1;
                    TIFFDataType dtype = TIFF_ASCII;
                    unsigned short bit = (unsigned short)(65 + i);
                    unsigned char oktochange = 1;
                    unsigned char passcount = 0;
                    char *name = NULL;

                    if (pos + 4 <= Size) {
                        tag = 65000U + (rd32(Data + pos) % 500U);
                        pos += 4;
                    }
                    if (pos + 2 <= Size) {
                        readcount = (short)((int)(rd16(Data + pos) % 5) - 2);
                        pos += 2;
                    }
                    if (pos + 2 <= Size) {
                        writecount = (short)((int)(rd16(Data + pos) % 5) - 2);
                        pos += 2;
                    }
                    if (pos < Size) {
                        switch (Data[pos] % 6) {
                            case 0: dtype = TIFF_BYTE; break;
                            case 1: dtype = TIFF_ASCII; break;
                            case 2: dtype = TIFF_SHORT; break;
                            case 3: dtype = TIFF_LONG; break;
                            case 4: dtype = TIFF_RATIONAL; break;
                            default: dtype = TIFF_UNDEFINED; break;
                        }
                        pos++;
                    }
                    if (pos + 2 <= Size) {
                        bit = (unsigned short)(rd16(Data + pos) % 256U);
                        pos += 2;
                    }
                    if (pos < Size) {
                        oktochange = (unsigned char)(Data[pos] & 1U);
                        pos++;
                    }
                    if (pos < Size) {
                        passcount = (unsigned char)(Data[pos] & 1U);
                        pos++;
                    }

                    name = (char *)malloc(32);
                    if (name != NULL) {
                        snprintf(name, 32, "fuzz_tag_%u", tag);
                    }

                    infos[i].field_tag = tag;
                    infos[i].field_readcount = readcount;
                    infos[i].field_writecount = writecount;
                    infos[i].field_type = dtype;
                    infos[i].field_bit = bit;
                    infos[i].field_oktochange = oktochange;
                    infos[i].field_passcount = passcount;
                    infos[i].field_name = name;
                }

                (void)TIFFMergeFieldInfo(tif, infos, count);

                if (Size > pos && (Data[pos] & 1U)) {
                    (void)TIFFCreateDirectory(tif);
                    (void)TIFFMergeFieldInfo(tif, infos, count);
                }
            }
        }
    }

    TIFFFreeDirectory(tif);

    if (infos != NULL) {
        for (i = 0; i < count; i++) {
            free((void *)infos[i].field_name);
        }
        free(infos);
    }

    TIFFCleanup(tif);
    return 0;
}