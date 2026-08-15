// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFRasterScanlineSize at tif_strip.c:337:1 in tiffio.h
// TIFFTileSize at tif_tile.c:257:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiffio.h"

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static tmsize_t clamp_alloc_size(tmsize_t v) {
    if (v <= 0) return 0;
    if (v > (tmsize_t)(1 << 20)) return (tmsize_t)(1 << 20);
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif;
    tmsize_t strip_size, scanline_size, raster_scanline_size, tile_size;
    uint32_t numstrips = 0, strip = 0;
    void *buf = NULL;
    void *rawbuf = NULL;
    tmsize_t read_sz;
    int mode_selector = 0;

    write_dummy_file(Data, Size);

    mode_selector = (Size > 0) ? (Data[0] % 3) : 0;
    if (mode_selector == 0) {
        tif = TIFFOpen("./dummy_file", "r");
    } else if (mode_selector == 1) {
        tif = TIFFOpen("./dummy_file", "rm");
    } else {
        tif = TIFFOpen("./dummy_file", "rC");
    }

    if (!tif) {
        return 0;
    }

    strip_size = TIFFStripSize(tif);
    scanline_size = TIFFScanlineSize(tif);
    raster_scanline_size = TIFFRasterScanlineSize(tif);
    tile_size = TIFFTileSize(tif);

    numstrips = TIFFNumberOfStrips(tif);
    if (numstrips > 0) {
        if (Size >= 5) {
            strip = ((uint32_t)Data[1] << 24) |
                    ((uint32_t)Data[2] << 16) |
                    ((uint32_t)Data[3] << 8) |
                    (uint32_t)Data[4];
            strip %= numstrips;
        } else {
            strip = 0;
        }

        read_sz = strip_size;
        if (Size >= 13) {
            uint64_t chooser = ((uint64_t)Data[5] << 56) |
                               ((uint64_t)Data[6] << 48) |
                               ((uint64_t)Data[7] << 40) |
                               ((uint64_t)Data[8] << 32) |
                               ((uint64_t)Data[9] << 24) |
                               ((uint64_t)Data[10] << 16) |
                               ((uint64_t)Data[11] << 8) |
                               (uint64_t)Data[12];
            switch (chooser % 5) {
                case 0:
                    read_sz = strip_size;
                    break;
                case 1:
                    read_sz = scanline_size;
                    break;
                case 2:
                    read_sz = raster_scanline_size;
                    break;
                case 3:
                    read_sz = tile_size;
                    break;
                default:
                    read_sz = (tmsize_t)(Size & 0xFFFF);
                    break;
            }
        }

        read_sz = clamp_alloc_size(read_sz);
        if (read_sz > 0) {
            buf = malloc((size_t)read_sz);
            if (buf) {
                memset(buf, 0, (size_t)read_sz);
                (void)TIFFReadEncodedStrip(tif, strip, buf, read_sz);
            }
        }

        if (Size > 13 && buf) {
            tmsize_t insize = (tmsize_t)(Size - 13);
            tmsize_t outsize = read_sz;
            if (insize > (tmsize_t)(1 << 20)) {
                insize = (tmsize_t)(1 << 20);
            }
            if (insize > 0) {
                rawbuf = malloc((size_t)insize);
                if (rawbuf) {
                    memcpy(rawbuf, Data + 13, (size_t)insize);
                    (void)TIFFReadFromUserBuffer(tif, strip, rawbuf, insize, buf, outsize);
                }
            }
        }

        if (buf) {
            free(buf);
            buf = NULL;
        }
        if (rawbuf) {
            free(rawbuf);
            rawbuf = NULL;
        }
    }

    TIFFClose(tif);
    return 0;
}