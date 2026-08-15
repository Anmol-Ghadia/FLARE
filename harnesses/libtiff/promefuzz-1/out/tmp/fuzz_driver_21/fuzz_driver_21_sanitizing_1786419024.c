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

static uint64_t read_u64(const uint8_t *p) {
    return ((uint64_t)p[0]) |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    uint16_t single_short = 0;
    uint32_t single_long = 0;

    if (Size >= 2) {
        single_short = read_u16(Data);
    } else if (Size == 1) {
        single_short = Data[0];
    }

    if (Size >= 4) {
        single_long = read_u32(Data);
    } else {
        for (size_t i = 0; i < Size; ++i) {
            single_long |= ((uint32_t)Data[i] << (8 * i));
        }
    }

    TIFFSwabShort(&single_short);
    TIFFSwabShort(&single_short);

    TIFFSwabLong(&single_long);
    TIFFSwabLong(&single_long);

    size_t max_short_elems = Size / 2;
    size_t max_long_elems = Size / 4;
    size_t max_long8_elems = Size / 8;
    size_t max_triple_elems = Size / 3;

    size_t short_count = 0;
    size_t long_count = 0;
    size_t long8_count = 0;
    size_t triple_count = 0;

    if (Size >= 1) {
        short_count = Data[0] % (max_short_elems + 1);
    }
    if (Size >= 2) {
        long_count = Data[1] % (max_long_elems + 1);
    }
    if (Size >= 3) {
        long8_count = Data[2] % (max_long8_elems + 1);
    }
    if (Size >= 4) {
        triple_count = Data[3] % (max_triple_elems + 1);
    }

    if (short_count > 0) {
        uint16_t *shorts = (uint16_t *)malloc(short_count * sizeof(uint16_t));
        if (shorts != NULL) {
            for (size_t i = 0; i < short_count; ++i) {
                size_t off = (i * 2) % Size;
                uint8_t b0 = Data[off];
                uint8_t b1 = Data[(off + 1) % Size];
                shorts[i] = (uint16_t)((uint16_t)b0 | ((uint16_t)b1 << 8));
            }
            TIFFSwabArrayOfShort(shorts, (tmsize_t)short_count);
            TIFFSwabArrayOfShort(shorts, (tmsize_t)short_count);
            free(shorts);
        }
    } else {
        TIFFSwabArrayOfShort(NULL, (tmsize_t)0);
    }

    if (long_count > 0) {
        uint32_t *longs = (uint32_t *)malloc(long_count * sizeof(uint32_t));
        if (longs != NULL) {
            for (size_t i = 0; i < long_count; ++i) {
                size_t off = (i * 4) % Size;
                uint8_t tmp[4];
                for (size_t j = 0; j < 4; ++j) {
                    tmp[j] = Data[(off + j) % Size];
                }
                longs[i] = read_u32(tmp);
            }
            TIFFSwabArrayOfLong(longs, (tmsize_t)long_count);
            TIFFSwabArrayOfLong(longs, (tmsize_t)long_count);
            free(longs);
        }
    } else {
        TIFFSwabArrayOfLong(NULL, (tmsize_t)0);
    }

    if (long8_count > 0) {
        uint64_t *long8s = (uint64_t *)malloc(long8_count * sizeof(uint64_t));
        if (long8s != NULL) {
            for (size_t i = 0; i < long8_count; ++i) {
                size_t off = (i * 8) % Size;
                uint8_t tmp[8];
                for (size_t j = 0; j < 8; ++j) {
                    tmp[j] = Data[(off + j) % Size];
                }
                long8s[i] = read_u64(tmp);
            }
            TIFFSwabArrayOfLong8(long8s, (tmsize_t)long8_count);
            TIFFSwabArrayOfLong8(long8s, (tmsize_t)long8_count);
            free(long8s);
        }
    } else {
        TIFFSwabArrayOfLong8(NULL, (tmsize_t)0);
    }

    if (triple_count > 0) {
        size_t bytes = triple_count * 3;
        uint8_t *triples = (uint8_t *)malloc(bytes);
        if (triples != NULL) {
            for (size_t i = 0; i < bytes; ++i) {
                triples[i] = Data[i % Size];
            }
            TIFFSwabArrayOfTriples(triples, (tmsize_t)triple_count);
            TIFFSwabArrayOfTriples(triples, (tmsize_t)triple_count);
            free(triples);
        }
    } else {
        TIFFSwabArrayOfTriples(NULL, (tmsize_t)0);
    }

    if (Size >= 12) {
        uint16_t s = read_u16(Data);
        uint32_t l = read_u32(Data + 2);
        uint64_t l8 = read_u64(Data + 4);

        TIFFSwabShort(&s);
        TIFFSwabLong(&l);

        uint16_t pair[2] = { s, (uint16_t)(l & 0xFFFFu) };
        uint32_t quad[2] = { l, (uint32_t)(l8 & 0xFFFFFFFFu) };
        uint64_t oct[1] = { l8 };
        uint8_t tri[3] = {
            (uint8_t)(s & 0xFFu),
            (uint8_t)(l & 0xFFu),
            (uint8_t)(l8 & 0xFFu)
        };

        TIFFSwabArrayOfShort(pair, (tmsize_t)2);
        TIFFSwabArrayOfLong(quad, (tmsize_t)2);
        TIFFSwabArrayOfLong8(oct, (tmsize_t)1);
        TIFFSwabArrayOfTriples(tri, (tmsize_t)1);
    }

    return 0;
}