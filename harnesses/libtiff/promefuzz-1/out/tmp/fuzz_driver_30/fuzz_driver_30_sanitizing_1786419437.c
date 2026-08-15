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

static uint16_t read_u16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    uint8_t *buf = NULL;
    uint8_t *triples = NULL;
    uint16_t *shorts = NULL;
    size_t i;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    if (Size == 0) {
        const unsigned char *tbl0 = TIFFGetBitRevTable(0);
        const unsigned char *tbl1 = TIFFGetBitRevTable(1);
        (void)tbl0;
        (void)tbl1;
        return 0;
    }

    buf = (uint8_t *)malloc(Size);
    if (buf == NULL) {
        return 0;
    }
    memcpy(buf, Data, Size);

    TIFFReverseBits(buf, (tmsize_t)Size);

    if (Size > 1) {
        tmsize_t n1 = (tmsize_t)(Data[0] % (Size + 1));
        TIFFReverseBits(buf, n1);
    }

    {
        const unsigned char *tbl_rev = TIFFGetBitRevTable((Size > 0) ? (Data[0] & 1) : 0);
        const unsigned char *tbl_nrev = TIFFGetBitRevTable(0);
        if (tbl_rev != NULL) {
            for (i = 0; i < Size; i++) {
                buf[i] = tbl_rev[buf[i]];
            }
        }
        if (tbl_nrev != NULL && Size > 0) {
            buf[0] = tbl_nrev[buf[0]];
        }
    }

    if (Size >= 4 && sizeof(float) == 4) {
        union {
            uint32_t u;
            float f;
        } fu;
        fu.u = read_u32(Data);
        TIFFSwabFloat(&fu.f);
        if (Size >= 8) {
            fu.u ^= read_u32(Data + 4);
            TIFFSwabFloat(&fu.f);
        }
    }

    if (Size >= 2) {
        uint16_t s = read_u16(Data);
        TIFFSwabShort(&s);
        if (Size >= 4) {
            s ^= read_u16(Data + 2);
            TIFFSwabShort(&s);
        }
    }

    {
        size_t short_count = Size / 2;
        if (short_count > 0 && sizeof(uint16_t) == 2) {
            shorts = (uint16_t *)malloc(short_count * sizeof(uint16_t));
            if (shorts != NULL) {
                for (i = 0; i < short_count; i++) {
                    shorts[i] = read_u16(Data + i * 2);
                }

                TIFFSwabArrayOfShort(shorts, (tmsize_t)short_count);

                if (short_count > 1) {
                    tmsize_t partial = (tmsize_t)(Data[0] % (short_count + 1));
                    TIFFSwabArrayOfShort(shorts, partial);
                }

                free(shorts);
                shorts = NULL;
            }
        }
    }

    {
        size_t triple_count = Size / 3;
        if (triple_count > 0) {
            triples = (uint8_t *)malloc(triple_count * 3);
            if (triples != NULL) {
                memcpy(triples, Data, triple_count * 3);

                TIFFSwabArrayOfTriples(triples, (tmsize_t)triple_count);

                if (triple_count > 1) {
                    tmsize_t partial = (tmsize_t)(Data[0] % (triple_count + 1));
                    TIFFSwabArrayOfTriples(triples, partial);
                }

                free(triples);
                triples = NULL;
            }
        }
    }

    free(buf);
    return 0;
}