// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFWriteRawStrip at tif_write.c:316:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadRawTile at tif_read.c:1083:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static void fuzz_read_paths(TIFF *tif, const uint8_t *Data, size_t Size) {
    if (!tif) return;

    uint32_t dir_loops = 1;
    if (Size > 0) {
        dir_loops = (uint32_t)(Data[0] % 4 + 1);
    }

    for (uint32_t d = 0; d < dir_loops; ++d) {
        uint32_t nstrips = TIFFNumberOfStrips(tif);

        uint32_t strip_candidates[6];
        strip_candidates[0] = 0;
        strip_candidates[1] = 1;
        strip_candidates[2] = nstrips;
        strip_candidates[3] = (nstrips > 0) ? (nstrips - 1) : 0;
        strip_candidates[4] = 0xFFFFFFFFU;
        strip_candidates[5] = (Size >= 5)
                                  ? ((uint32_t)Data[1] |
                                     ((uint32_t)Data[2] << 8) |
                                     ((uint32_t)Data[3] << 16) |
                                     ((uint32_t)Data[4] << 24))
                                  : 2U;

        for (size_t i = 0; i < sizeof(strip_candidates) / sizeof(strip_candidates[0]); ++i) {
            uint32_t strip = strip_candidates[i];
            tmsize_t rawsz = TIFFRawStripSize(tif, strip);

            tmsize_t allocsz = 4096;
            if (rawsz > 0 && rawsz < (tmsize_t)(1 << 20)) {
                allocsz = rawsz;
            }
            if (allocsz <= 0) {
                allocsz = 4096;
            }

            void *buf = malloc((size_t)allocsz);
            if (!buf) continue;
            memset(buf, 0, (size_t)allocsz);

            (void)TIFFReadRawStrip(tif, strip, buf, (tmsize_t)-1);
            (void)TIFFReadRawStrip(tif, strip, buf, allocsz / 2);
            (void)TIFFReadRawStrip(tif, strip, buf, allocsz);

            (void)TIFFReadEncodedStrip(tif, strip, buf, (tmsize_t)-1);
            (void)TIFFReadEncodedStrip(tif, strip, buf, allocsz / 2);
            (void)TIFFReadEncodedStrip(tif, strip, buf, allocsz);

            free(buf);
        }

        uint32_t tile_candidates[5];
        tile_candidates[0] = 0;
        tile_candidates[1] = 1;
        tile_candidates[2] = 0xFFFFFFFFU;
        tile_candidates[3] = (Size >= 9)
                                 ? ((uint32_t)Data[5] |
                                    ((uint32_t)Data[6] << 8) |
                                    ((uint32_t)Data[7] << 16) |
                                    ((uint32_t)Data[8] << 24))
                                 : 3U;
        tile_candidates[4] = nstrips;

        for (size_t i = 0; i < sizeof(tile_candidates) / sizeof(tile_candidates[0]); ++i) {
            uint32_t tile = tile_candidates[i];
            size_t bufsz = 4096;
            void *buf = malloc(bufsz);
            if (!buf) continue;
            memset(buf, 0, bufsz);

            (void)TIFFReadRawTile(tif, tile, buf, (tmsize_t)-1);
            (void)TIFFReadRawTile(tif, tile, buf, (tmsize_t)(bufsz / 2));
            (void)TIFFReadRawTile(tif, tile, buf, (tmsize_t)bufsz);

            free(buf);
        }

        if (!TIFFReadDirectory(tif)) {
            break;
        }
    }
}

static void fuzz_write_paths(const uint8_t *Data, size_t Size) {
    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return;

    uint32_t width = 1 + (Size > 0 ? Data[0] : 0);
    uint32_t height = 1 + (Size > 1 ? Data[1] : 0);
    uint16_t spp = 1;
    uint16_t bps = 8;
    uint16_t planar = (Size > 2 && (Data[2] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = PHOTOMETRIC_MINISBLACK;
    uint32_t rowsperstrip = (height > 0) ? ((Size > 3 ? Data[3] : 1) % height + 1) : 1;

    if (!TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width) ||
        !TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height) ||
        !TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp) ||
        !TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps) ||
        !TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar) ||
        !TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric) ||
        !TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip) ||
        !TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE)) {
        TIFFClose(tif);
        return;
    }

    size_t payload_size = Size;
    if (payload_size == 0) payload_size = 1;

    void *buf = malloc(payload_size);
    if (!buf) {
        TIFFClose(tif);
        return;
    }

    if (Size > 0) {
        memcpy(buf, Data, Size);
    } else {
        ((uint8_t *)buf)[0] = 0;
    }

    uint32_t nstrips = TIFFNumberOfStrips(tif);
    if (nstrips == 0) {
        free(buf);
        TIFFClose(tif);
        return;
    }

    uint32_t valid_strip = 0;
    uint32_t last_valid_strip = nstrips - 1;

    (void)TIFFWriteRawStrip(tif, valid_strip, buf, (tmsize_t)payload_size);
    (void)TIFFWriteRawStrip(tif, valid_strip, buf, (tmsize_t)(payload_size / 2));
    (void)TIFFWriteRawStrip(tif, valid_strip, buf, 0);

    if (last_valid_strip != valid_strip) {
        (void)TIFFWriteRawStrip(tif, last_valid_strip, buf, (tmsize_t)payload_size);
        (void)TIFFWriteRawStrip(tif, last_valid_strip, buf, (tmsize_t)(payload_size / 2));
        (void)TIFFWriteRawStrip(tif, last_valid_strip, buf, 0);
    }

    free(buf);
    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        fuzz_read_paths(tif, Data, Size);
        TIFFClose(tif);
    }

    fuzz_write_paths(Data, Size);
    return 0;
}