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

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int bounded_int(uint8_t v, int maxv) {
    if (maxv <= 0) return 0;
    return (int)(v % (uint8_t)maxv);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif = NULL;
    TIFFFieldInfo *infos = NULL;
    char *namebuf = NULL;
    uint32_t ninfos = 0;
    size_t i;
    size_t off = 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);

    if (Size > 0 && (Data[0] & 1))
        tif = TIFFOpen("./dummy_file", "r");
    else
        tif = TIFFOpen("./dummy_file", "r+");

    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
        if (!tif)
            return 0;
    }

    if (Size >= 1) {
        ninfos = (uint32_t)(Data[0] % 8);
    }

    if (ninfos > 0) {
        infos = (TIFFFieldInfo *)calloc(ninfos, sizeof(TIFFFieldInfo));
        if (infos) {
            off = 1;
            for (i = 0; i < ninfos; i++) {
                uint32_t tag = 65000U + (uint32_t)i;
                short rc = 1;
                short wc = 1;
                TIFFDataType dtype = TIFF_ASCII;
                int dtpick = 0;
                char *nm;

                if (off + 8 <= Size) {
                    tag = rd16(Data + off);
                    off += 2;
                    rc = (short)((int8_t)Data[off++]);
                    wc = (short)((int8_t)Data[off++]);
                    dtpick = bounded_int(Data[off++], 18);
                    off += 3;
                }

                switch (dtpick) {
                    case 0: dtype = TIFF_NOTYPE; break;
                    case 1: dtype = TIFF_BYTE; break;
                    case 2: dtype = TIFF_ASCII; break;
                    case 3: dtype = TIFF_SHORT; break;
                    case 4: dtype = TIFF_LONG; break;
                    case 5: dtype = TIFF_RATIONAL; break;
                    case 6: dtype = TIFF_SBYTE; break;
                    case 7: dtype = TIFF_UNDEFINED; break;
                    case 8: dtype = TIFF_SSHORT; break;
                    case 9: dtype = TIFF_SLONG; break;
                    case 10: dtype = TIFF_SRATIONAL; break;
                    case 11: dtype = TIFF_FLOAT; break;
                    case 12: dtype = TIFF_DOUBLE; break;
#ifdef TIFF_IFD
                    case 13: dtype = TIFF_IFD; break;
#endif
#ifdef TIFF_LONG8
                    case 14: dtype = TIFF_LONG8; break;
#endif
#ifdef TIFF_SLONG8
                    case 15: dtype = TIFF_SLONG8; break;
#endif
#ifdef TIFF_IFD8
                    case 16: dtype = TIFF_IFD8; break;
#endif
                    default: dtype = TIFF_ASCII; break;
                }

                nm = (char *)malloc(32);
                if (!nm)
                    continue;
                snprintf(nm, 32, "FuzzTag_%u_%zu", tag, i);

                infos[i].field_tag = tag;
                infos[i].field_readcount = rc;
                infos[i].field_writecount = wc;
                infos[i].field_type = dtype;
                infos[i].field_bit = (unsigned short)(i % 64);
                infos[i].field_oktochange = (unsigned char)(i & 1);
                infos[i].field_passcount = (unsigned char)((i >> 1) & 1);
                infos[i].field_name = nm;
            }

            (void)TIFFMergeFieldInfo(tif, infos, ninfos);
        }
    }

    if (Size >= 4) {
        uint32_t tag = rd32(Data);
        TIFFDataType qtype = TIFF_ANY;
        const TIFFField *fld;

        if (Size >= 5) {
            switch (bounded_int(Data[4], 6)) {
                case 0: qtype = TIFF_ANY; break;
                case 1: qtype = TIFF_ASCII; break;
                case 2: qtype = TIFF_SHORT; break;
                case 3: qtype = TIFF_LONG; break;
                case 4: qtype = TIFF_BYTE; break;
                default: qtype = TIFF_NOTYPE; break;
            }
        }

        fld = TIFFFindField(tif, tag, qtype);
        if (fld) {
            (void)TIFFFieldName(fld);
            (void)TIFFFieldTag(fld);
        }

        fld = TIFFFieldWithTag(tif, tag);
        if (fld) {
            (void)TIFFFieldName(fld);
            (void)TIFFFieldTag(fld);
        }
    }

    if (Size > 0) {
        size_t namelen = Size > 64 ? 64 : Size;
        namebuf = (char *)malloc(namelen + 1);
        if (namebuf) {
            for (i = 0; i < namelen; i++) {
                unsigned char c = Data[i];
                if (c == 0 || c == '\n' || c == '\r')
                    c = 'A';
                namebuf[i] = (char)c;
            }
            namebuf[namelen] = '\0';

            {
                const TIFFField *fld = TIFFFieldWithName(tif, namebuf);
                if (fld) {
                    (void)TIFFFieldName(fld);
                    (void)TIFFFieldTag(fld);
                }
            }
        }
    }

    if (infos) {
        for (i = 0; i < ninfos; i++) {
            const TIFFField *fld;

            if (infos[i].field_name) {
                fld = TIFFFieldWithName(tif, infos[i].field_name);
                if (fld) {
                    (void)TIFFFieldName(fld);
                    (void)TIFFFieldTag(fld);
                }
            }

            fld = TIFFFindField(tif, infos[i].field_tag, infos[i].field_type);
            if (fld) {
                (void)TIFFFieldName(fld);
                (void)TIFFFieldTag(fld);
            }

            fld = TIFFFieldWithTag(tif, infos[i].field_tag);
            if (fld) {
                (void)TIFFFieldName(fld);
                (void)TIFFFieldTag(fld);
            }
        }
    }

    if (infos) {
        for (i = 0; i < ninfos; i++) {
            free((void *)infos[i].field_name);
        }
        free(infos);
    }

    free(namebuf);

    if (tif)
        TIFFClose(tif);

    return 0;
}