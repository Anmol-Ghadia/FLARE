// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFFlushData at tif_flush.c:155:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFFlushData at tif_flush.c:155:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFCreateDirectory at tif_dir.c:1365:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFDeferStrileArrayWriting at tif_dirwrite.c:224:5 in tiffio.h
// TIFFFlushData at tif_flush.c:155:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Data && Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint16_t rd16(const uint8_t *p, size_t n, size_t off, uint16_t defv) {
    if (off + 1 >= n) return defv;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off, uint32_t defv) {
    if (off + 3 >= n) return defv;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

int LLVMFuzzerTestOneInput_41(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif_rw = TIFFOpen("./dummy_file", "w8");
    if (tif_rw) {
        (void)TIFFCreateDirectory(tif_rw);

        uint32_t width = (Size > 0) ? ((uint32_t)Data[0] + 1U) : 1U;
        uint32_t height = (Size > 1) ? ((uint32_t)Data[1] + 1U) : 1U;
        uint16_t spp = (Size > 2) ? (uint16_t)((Data[2] % 4U) + 1U) : 1U;
        uint16_t bps_choices[] = {1, 4, 8, 16};
        uint16_t bps = bps_choices[(Size > 3) ? (Data[3] % 4U) : 2U];
        uint16_t photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
        uint16_t planar = (Size > 4 && (Data[4] & 1U)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        uint16_t compression = COMPRESSION_NONE;
        uint32_t rowsperstrip = (Size > 5) ? ((uint32_t)Data[5] + 1U) : 1U;

        (void)TIFFSetField(tif_rw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif_rw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif_rw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif_rw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif_rw, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif_rw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tif_rw, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tif_rw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        if (photo == PHOTOMETRIC_RGB) {
            (void)TIFFSetField(tif_rw, TIFFTAG_EXTRASAMPLES, 0, NULL);
        }

        (void)TIFFDeferStrileArrayWriting(tif_rw);
        (void)TIFFFlushData(tif_rw);
        (void)TIFFCheckpointDirectory(tif_rw);
        (void)TIFFForceStrileArrayWriting(tif_rw);
        (void)TIFFWriteDirectory(tif_rw);

        (void)TIFFCreateDirectory(tif_rw);

        width = rd32(Data, Size, 6, 16U) % 1024U + 1U;
        height = rd32(Data, Size, 10, 16U) % 1024U + 1U;
        spp = (uint16_t)((rd16(Data, Size, 14, 1U) % 4U) + 1U);
        bps = bps_choices[(Size > 16) ? (Data[16] % 4U) : 2U];
        photo = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISWHITE;
        planar = (Size > 17 && (Data[17] & 1U)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
        rowsperstrip = rd32(Data, Size, 18, 8U) % 256U + 1U;

        (void)TIFFSetField(tif_rw, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif_rw, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif_rw, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif_rw, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif_rw, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif_rw, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tif_rw, TIFFTAG_COMPRESSION, compression);
        (void)TIFFSetField(tif_rw, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        if (Size > 22 && (Data[22] & 1U)) {
            (void)TIFFDeferStrileArrayWriting(tif_rw);
        }
        (void)TIFFFlushData(tif_rw);
        (void)TIFFCheckpointDirectory(tif_rw);
        (void)TIFFForceStrileArrayWriting(tif_rw);
        (void)TIFFWriteDirectory(tif_rw);

        TIFFClose(tif_rw);
    }

    TIFF *tif_ro = TIFFOpen("./dummy_file", "r");
    if (tif_ro) {
        (void)TIFFCreateDirectory(tif_ro);
        (void)TIFFDeferStrileArrayWriting(tif_ro);
        (void)TIFFForceStrileArrayWriting(tif_ro);
        (void)TIFFFlushData(tif_ro);
        (void)TIFFCheckpointDirectory(tif_ro);
        (void)TIFFWriteDirectory(tif_ro);
        TIFFClose(tif_ro);
    }

    return 0;
}