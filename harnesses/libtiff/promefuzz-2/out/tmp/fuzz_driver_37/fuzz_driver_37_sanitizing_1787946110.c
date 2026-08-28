#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

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
    if (Data == NULL) {
        return 0;
    }

    /* Exercise TIFFSwabLong on a single 32-bit value. */
    {
        uint32_t v = 0;
        if (Size >= 4) {
            v = read_u32(Data);
        } else {
            size_t i;
            for (i = 0; i < Size; ++i) {
                v |= ((uint32_t)Data[i]) << (8 * i);
            }
        }
        TIFFSwabLong(&v);
        TIFFSwabLong(&v);
    }

    /* Exercise array-based swab functions with diverse lengths and contents. */
    {
        size_t offset = 0;
        size_t count32 = Size / 4;
        size_t count64 = Size / 8;
        size_t countTriple = Size / 3;
        size_t countFloat = Size / 4;
        size_t countDouble = Size / 8;

        if (count32 > 0) {
            uint32_t *arr32 = (uint32_t *)malloc(count32 * sizeof(uint32_t));
            if (arr32 != NULL) {
                size_t i;
                for (i = 0; i < count32; ++i) {
                    arr32[i] = read_u32(Data + i * 4);
                }

                TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);
                if (count32 > 1) {
                    TIFFSwabArrayOfLong(arr32, (tmsize_t)(count32 / 2));
                    TIFFSwabArrayOfLong(arr32 + (count32 / 2),
                                        (tmsize_t)(count32 - count32 / 2));
                }
                TIFFSwabArrayOfLong(arr32, 0);
                free(arr32);
            }
        } else {
            TIFFSwabArrayOfLong(NULL, 0);
        }

        if (count64 > 0) {
            uint64_t *arr64 = (uint64_t *)malloc(count64 * sizeof(uint64_t));
            if (arr64 != NULL) {
                size_t i;
                for (i = 0; i < count64; ++i) {
                    arr64[i] = read_u64(Data + i * 8);
                }

                TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);
                if (count64 > 1) {
                    TIFFSwabArrayOfLong8(arr64, (tmsize_t)(count64 / 2));
                    TIFFSwabArrayOfLong8(arr64 + (count64 / 2),
                                         (tmsize_t)(count64 - count64 / 2));
                }
                TIFFSwabArrayOfLong8(arr64, 0);
                free(arr64);
            }
        } else {
            TIFFSwabArrayOfLong8(NULL, 0);
        }

        if (countTriple > 0) {
            size_t tripleBytes = countTriple * 3;
            uint8_t *triples = (uint8_t *)malloc(tripleBytes);
            if (triples != NULL) {
                memcpy(triples, Data, tripleBytes);
                TIFFSwabArrayOfTriples(triples, (tmsize_t)countTriple);
                if (countTriple > 1) {
                    TIFFSwabArrayOfTriples(triples, (tmsize_t)(countTriple / 2));
                    TIFFSwabArrayOfTriples(
                        triples + (countTriple / 2) * 3,
                        (tmsize_t)(countTriple - countTriple / 2));
                }
                TIFFSwabArrayOfTriples(triples, 0);
                free(triples);
            }
        } else {
            TIFFSwabArrayOfTriples(NULL, 0);
        }

        if (countFloat > 0) {
            float *arrf = (float *)malloc(countFloat * sizeof(float));
            if (arrf != NULL) {
                size_t i;
                for (i = 0; i < countFloat; ++i) {
                    uint32_t bits = read_u32(Data + i * 4);
                    memcpy(&arrf[i], &bits, sizeof(float));
                }

                TIFFSwabArrayOfFloat(arrf, (tmsize_t)countFloat);
                if (countFloat > 1) {
                    TIFFSwabArrayOfFloat(arrf, (tmsize_t)(countFloat / 2));
                    TIFFSwabArrayOfFloat(arrf + (countFloat / 2),
                                         (tmsize_t)(countFloat - countFloat / 2));
                }
                TIFFSwabArrayOfFloat(arrf, 0);
                free(arrf);
            }
        } else {
            TIFFSwabArrayOfFloat(NULL, 0);
        }

        if (countDouble > 0) {
            double *arrd = (double *)malloc(countDouble * sizeof(double));
            if (arrd != NULL) {
                size_t i;
                for (i = 0; i < countDouble; ++i) {
                    uint64_t bits = read_u64(Data + i * 8);
                    memcpy(&arrd[i], &bits, sizeof(double));
                }

                TIFFSwabArrayOfDouble(arrd, (tmsize_t)countDouble);
                if (countDouble > 1) {
                    TIFFSwabArrayOfDouble(arrd, (tmsize_t)(countDouble / 2));
                    TIFFSwabArrayOfDouble(arrd + (countDouble / 2),
                                          (tmsize_t)(countDouble - countDouble / 2));
                }
                TIFFSwabArrayOfDouble(arrd, 0);
                free(arrd);
            }
        } else {
            TIFFSwabArrayOfDouble(NULL, 0);
        }

        /* Mixed-state exploration using a rolling offset and bounded counts. */
        if (Size > 0) {
            size_t smallCount = Data[0] % 16;
            offset = 1;

            if (smallCount > 0 && offset + smallCount * 4 <= Size) {
                uint32_t *mix32 = (uint32_t *)malloc(smallCount * sizeof(uint32_t));
                if (mix32 != NULL) {
                    size_t i;
                    for (i = 0; i < smallCount; ++i) {
                        mix32[i] = read_u32(Data + offset + i * 4);
                    }
                    TIFFSwabArrayOfLong(mix32, (tmsize_t)smallCount);
                    free(mix32);
                }
                offset += smallCount * 4;
            }

            if (offset < Size) {
                size_t remain = Size - offset;
                size_t smallTriples = remain / 3;
                if (smallTriples > 16) {
                    smallTriples = 16;
                }
                if (smallTriples > 0) {
                    uint8_t *mixTriples = (uint8_t *)malloc(smallTriples * 3);
                    if (mixTriples != NULL) {
                        memcpy(mixTriples, Data + offset, smallTriples * 3);
                        TIFFSwabArrayOfTriples(mixTriples, (tmsize_t)smallTriples);
                        free(mixTriples);
                    }
                }
            }
        }
    }

    return 0;
}