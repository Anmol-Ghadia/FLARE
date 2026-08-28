// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// TIFFSwabArrayOfLong at tif_swab.c:103:1 in tiffio.h
// TIFFSwabArrayOfLong8 at tif_swab.c:120:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabLong at tif_swab.c:45:1 in tiffio.h
// _TIFFmemcpy at tif_unix.c:344:1 in tiffio.h
// TIFFSwabFloat at tif_swab.c:139:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
// TIFFSwabArrayOfShort at tif_swab.c:71:1 in tiffio.h
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
    return (uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8);
}

static uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t read_u64(const uint8_t *p) {
    return (uint64_t)p[0] |
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

    if (Size == 0) {
        uint32_t v32 = 0;
        uint64_t v64 = 0;
        uint16_t v16 = 0;
        float vf = 0.0f;
        TIFFSwabLong(&v32);
        TIFFSwabArrayOfLong(&v32, 0);
        TIFFSwabArrayOfLong8(&v64, 0);
        TIFFSwabArrayOfShort(&v16, 0);
        TIFFSwabFloat(&vf);
        return 0;
    }

    size_t off = 0;

    if (Size >= 4) {
        uint32_t v32 = read_u32(Data);
        TIFFSwabLong(&v32);
    }

    if (Size >= 4) {
        uint32_t fv = read_u32(Data + (Size >= 8 ? 4 : 0));
        float f;
        _TIFFmemcpy(&f, &fv, (tmsize_t)sizeof(float));
        TIFFSwabFloat(&f);
    }

    {
        size_t count16 = Size / 2;
        if (count16 > 0) {
            uint16_t *arr16 = (uint16_t *)malloc(count16 * sizeof(uint16_t));
            if (arr16 != NULL) {
                size_t i;
                for (i = 0; i < count16; i++) {
                    arr16[i] = read_u16(Data + i * 2);
                }
                TIFFSwabArrayOfShort(arr16, (tmsize_t)count16);
                if (count16 > 1) {
                    TIFFSwabArrayOfShort(arr16, (tmsize_t)(count16 / 2));
                }
                free(arr16);
            }
        } else {
            uint16_t dummy16 = 0;
            TIFFSwabArrayOfShort(&dummy16, 0);
        }
    }

    {
        size_t count32 = Size / 4;
        if (count32 > 0) {
            uint32_t *arr32 = (uint32_t *)malloc(count32 * sizeof(uint32_t));
            if (arr32 != NULL) {
                size_t i;
                for (i = 0; i < count32; i++) {
                    arr32[i] = read_u32(Data + i * 4);
                }
                TIFFSwabArrayOfLong(arr32, (tmsize_t)count32);
                if (count32 > 1) {
                    TIFFSwabArrayOfLong(arr32, (tmsize_t)(count32 - 1));
                }
                free(arr32);
            }
        } else {
            uint32_t dummy32 = 0;
            TIFFSwabArrayOfLong(&dummy32, 0);
        }
    }

    {
        size_t count64 = Size / 8;
        if (count64 > 0) {
            uint64_t *arr64 = (uint64_t *)malloc(count64 * sizeof(uint64_t));
            if (arr64 != NULL) {
                size_t i;
                for (i = 0; i < count64; i++) {
                    arr64[i] = read_u64(Data + i * 8);
                }
                TIFFSwabArrayOfLong8(arr64, (tmsize_t)count64);
                if (count64 > 1) {
                    TIFFSwabArrayOfLong8(arr64, (tmsize_t)(count64 / 2));
                }
                free(arr64);
            }
        } else {
            uint64_t dummy64 = 0;
            TIFFSwabArrayOfLong8(&dummy64, 0);
        }
    }

    {
        size_t copy_len = Size;
        uint8_t *src = (uint8_t *)malloc(copy_len == 0 ? 1 : copy_len);
        uint8_t *dst = (uint8_t *)malloc(copy_len == 0 ? 1 : copy_len);
        if (src != NULL && dst != NULL) {
            _TIFFmemcpy(src, Data, (tmsize_t)copy_len);
            _TIFFmemcpy(dst, src, (tmsize_t)copy_len);

            if (copy_len >= 4) {
                uint32_t tmp32;
                _TIFFmemcpy(&tmp32, dst, (tmsize_t)sizeof(tmp32));
                TIFFSwabLong(&tmp32);
            }
            if (copy_len >= 2) {
                uint16_t tmp16;
                _TIFFmemcpy(&tmp16, dst, (tmsize_t)sizeof(tmp16));
                TIFFSwabArrayOfShort(&tmp16, 1);
            }
            if (copy_len >= 8) {
                uint64_t tmp64;
                _TIFFmemcpy(&tmp64, dst, (tmsize_t)sizeof(tmp64));
                TIFFSwabArrayOfLong8(&tmp64, 1);
            }
            if (copy_len >= 4) {
                float tmpf;
                _TIFFmemcpy(&tmpf, dst, (tmsize_t)sizeof(tmpf));
                TIFFSwabFloat(&tmpf);
            }
        }
        free(src);
        free(dst);
    }

    while (off + 1 < Size) {
        uint8_t selector = Data[off++];
        uint8_t lenbyte = Data[off++];
        size_t remaining = Size - off;
        size_t chunk = (size_t)lenbyte;
        if (chunk > remaining) {
            chunk = remaining;
        }

        switch (selector % 6) {
            case 0:
                if (chunk >= 4) {
                    uint32_t v = read_u32(Data + off);
                    TIFFSwabLong(&v);
                }
                break;
            case 1:
                if (chunk >= 4) {
                    uint32_t raw = read_u32(Data + off);
                    float f;
                    _TIFFmemcpy(&f, &raw, (tmsize_t)sizeof(f));
                    TIFFSwabFloat(&f);
                }
                break;
            case 2:
                if (chunk > 0) {
                    uint8_t *buf = (uint8_t *)malloc(chunk);
                    if (buf != NULL) {
                        _TIFFmemcpy(buf, Data + off, (tmsize_t)chunk);
                        free(buf);
                    }
                }
                break;
            case 3:
                if (chunk >= 2) {
                    size_t n = chunk / 2;
                    uint16_t *a = (uint16_t *)malloc(n * sizeof(uint16_t));
                    if (a != NULL) {
                        size_t i;
                        for (i = 0; i < n; i++) {
                            a[i] = read_u16(Data + off + i * 2);
                        }
                        TIFFSwabArrayOfShort(a, (tmsize_t)n);
                        free(a);
                    }
                }
                break;
            case 4:
                if (chunk >= 4) {
                    size_t n = chunk / 4;
                    uint32_t *a = (uint32_t *)malloc(n * sizeof(uint32_t));
                    if (a != NULL) {
                        size_t i;
                        for (i = 0; i < n; i++) {
                            a[i] = read_u32(Data + off + i * 4);
                        }
                        TIFFSwabArrayOfLong(a, (tmsize_t)n);
                        free(a);
                    }
                }
                break;
            case 5:
                if (chunk >= 8) {
                    size_t n = chunk / 8;
                    uint64_t *a = (uint64_t *)malloc(n * sizeof(uint64_t));
                    if (a != NULL) {
                        size_t i;
                        for (i = 0; i < n; i++) {
                            a[i] = read_u64(Data + off + i * 8);
                        }
                        TIFFSwabArrayOfLong8(a, (tmsize_t)n);
                        free(a);
                    }
                }
                break;
        }

        off += chunk;
    }

    return 0;
}