#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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

static TIFFDataType clamp_dtype(uint8_t v) {
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
    FILE *fp = fopen("./dummy_file", "wb");
    TIFF *tif = NULL;
    const TIFFField *field = NULL;

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
        static char names[3][33];
        size_t off = 0;
        uint32_t n = 1;
        uint32_t i;

        memset(finfo, 0, sizeof(finfo));
        memset(names, 0, sizeof(names));

        if (Size > 0) {
            n = (uint32_t)(Data[0] % 3) + 1;
            off = 1;
        }

        for (i = 0; i < n; i++) {
            uint16_t tag = (uint16_t)(65000 + i);
            TIFFDataType dtype = TIFF_ANY;
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

            if (namelen > 0) {
                memcpy(names[i], Data + off, namelen);
            }
            names[i][namelen] = '\0';
            off += namelen;

            if (names[i][0] == '\0') {
                (void)snprintf(names[i], sizeof(names[i]), "Tag%u", (unsigned)i);
            }

            finfo[i].field_tag = tag;
            finfo[i].field_readcount = TIFF_VARIABLE2;
            finfo[i].field_writecount = TIFF_VARIABLE2;
            finfo[i].field_type = dtype;
            finfo[i].field_bit = FIELD_CUSTOM;
            finfo[i].field_oktochange = 1;
            finfo[i].field_passcount = (unsigned char)(i & 1);
            finfo[i].field_name = names[i];
        }

        (void)TIFFMergeFieldInfo(tif, finfo, n);

        for (i = 0; i < n; i++) {
            field = TIFFFieldWithName(tif, finfo[i].field_name);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
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
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }

            field = TIFFFindField(tif, tag, TIFF_ANY);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }
        }

        if (Size > 0) {
            char query[65];
            size_t qlen = Size > 64 ? 64 : Size;

            memcpy(query, Data, qlen);
            query[qlen] = '\0';

            field = TIFFFieldWithName(tif, query);
            if (field != NULL) {
                (void)TIFFFieldName(field);
                (void)TIFFFieldTag(field);
            }
        }
    }

    TIFFClose(tif);
    return 0;
}