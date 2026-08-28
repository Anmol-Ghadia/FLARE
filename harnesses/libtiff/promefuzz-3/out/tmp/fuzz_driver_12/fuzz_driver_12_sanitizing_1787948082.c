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

#include <tiffio.h>

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int clamp_dtype(uint8_t v) {
    switch (v % 8) {
        case 0: return TIFF_BYTE;
        case 1: return TIFF_ASCII;
        case 2: return TIFF_SHORT;
        case 3: return TIFF_LONG;
        case 4: return TIFF_RATIONAL;
        case 5: return TIFF_SBYTE;
        case 6: return TIFF_UNDEFINED;
        default: return TIFF_ANY;
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    const TIFFField *field = NULL;
    const char *name = NULL;

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

    {
        TIFFFieldInfo finfo[3];
        char *names[3] = { NULL, NULL, NULL };
        size_t off = 0;
        uint32_t n = 0;
        uint32_t i;

        memset(finfo, 0, sizeof(finfo));

        if (Size > 0) {
            n = (uint32_t)(Data[0] % 3) + 1;
            off = 1;
        } else {
            n = 1;
        }

        for (i = 0; i < n; i++) {
            uint16_t tag = (uint16_t)(65000 + i);
            int dtype = TIFF_ANY;
            size_t remain;
            size_t namelen;

            if (off + 2 <= Size) {
                tag = read_u16(Data + off);
                off += 2;
            }
            if (off < Size) {
                dtype = clamp_dtype(Data[off]);
                off++;
            }

            remain = (off < Size) ? (Size - off) : 0;
            namelen = remain > 32 ? 32 : remain;

            names[i] = (char *)malloc(namelen + 1);
            if (names[i] == NULL) {
                continue;
            }
            if (namelen > 0) {
                memcpy(names[i], Data + off, namelen);
            }
            names[i][namelen] = '\0';
            off += namelen;

            finfo[i].field_tag = tag;
            finfo[i].field_readcount = TIFF_VARIABLE2;
            finfo[i].field_writecount = TIFF_VARIABLE2;
            finfo[i].field_type = (TIFFDataType)dtype;
            finfo[i].field_bit = FIELD_CUSTOM;
            finfo[i].field_oktochange = 1;
            finfo[i].field_passcount = (unsigned char)(i & 1);
            finfo[i].field_name = names[i];
        }

        (void)TIFFMergeFieldInfo(tif, finfo, n);

        for (i = 0; i < n; i++) {
            if (names[i] != NULL && names[i][0] != '\0') {
                field = TIFFFieldWithName(tif, names[i]);
                if (field != NULL) {
                    (void)TIFFFieldName(field);
                    (void)TIFFFieldTag(field);
                }
            }

            field = TIFFFieldWithTag(tif, finfo[i].field_tag);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }

            field = TIFFFindField(tif, finfo[i].field_tag, finfo[i].field_type);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }

            field = TIFFFindField(tif, finfo[i].field_tag, TIFF_ANY);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }
        }

        if (Size > 4) {
            uint32_t tag = read_u32(Data + (Size - 4));
            field = TIFFFieldWithTag(tif, tag);
            if (field != NULL) {
                name = TIFFFieldName(field);
                (void)name;
                (void)TIFFFieldTag(field);
            }
            field = TIFFFindField(tif, tag, TIFF_ANY);
            if (field != NULL) {
                name = TIFFFieldName(field);
                (void)name;
                (void)TIFFFieldTag(field);
            }
        }

        if (Size > 0) {
            size_t namelen = Size > 64 ? 64 : Size;
            char *query = (char *)malloc(namelen + 1);
            if (query != NULL) {
                memcpy(query, Data, namelen);
                query[namelen] = '\0';
                field = TIFFFieldWithName(tif, query);
                if (field != NULL) {
                    (void)TIFFFieldName(field);
                    (void)TIFFFieldTag(field);
                }
                free(query);
            }
        }

        for (i = 0; i < 3; i++) {
            free(names[i]);
        }
    }

    TIFFClose(tif);
    return 0;
}