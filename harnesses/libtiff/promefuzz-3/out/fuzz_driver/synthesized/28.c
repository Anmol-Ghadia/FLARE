// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFFlushData at tif_flush.c:155:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFFlush at tif_flush.c:31:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFRewriteDirectory at tif_dirwrite.c:275:1 in tiffio.h
// TIFFWriteCheck at tif_write.c:572:1 in tiffio.h
// TIFFFlushData at tif_flush.c:155:1 in tiffio.h
// TIFFCheckpointDirectory at tif_dirwrite.c:251:1 in tiffio.h
// TIFFFlush at tif_flush.c:31:1 in tiffio.h
// TIFFForceStrileArrayWriting at tif_flush.c:78:5 in tiffio.h
// TIFFRewriteDirectory at tif_dirwrite.c:275:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <tiffio.h>

static uint16_t rd16(const uint8_t *p, size_t n)
{
    if (n < 2) return 0;
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n)
{
    if (n < 4) return 0;
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_28(const uint8_t *Data, size_t Size)
{
    FILE *fp;
    TIFF *tif = NULL;
    uint32_t width, height;
    uint16_t spp, bps, photo, config, planar, orient;
    uint32_t rowsperstrip;
    int use_tiles, mode_sel;
    int is_big;
    const char *mode;
    size_t off = 0;
    int ret;

    fp = fopen("./dummy_file", "wb");
    if (!fp)
        return 0;
    if (Size > 0)
        (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

    mode_sel = (Size > 0) ? (Data[0] % 5) : 0;
    is_big = (Size > 1) ? (Data[1] & 1) : 0;

    switch (mode_sel)
    {
        case 0: mode = is_big ? "w8" : "w"; break;
        case 1: mode = is_big ? "w8" : "w"; break;
        case 2: mode = "r+"; break;
        case 3: mode = "a"; break;
        default: mode = "r"; break;
    }

    tif = TIFFOpen("./dummy_file", mode);
    if (!tif)
        return 0;

    if (Size >= off + 4) { width = rd32(Data + off, Size - off); off += 4; }
    else width = 1;
    if (Size >= off + 4) { height = rd32(Data + off, Size - off); off += 4; }
    else height = 1;
    if (Size >= off + 2) { spp = (uint16_t)((rd16(Data + off, Size - off) % 4) + 1); off += 2; }
    else spp = 1;
    if (Size >= off + 2)
    {
        uint16_t vals[] = {1, 2, 4, 8, 16, 32};
        bps = vals[rd16(Data + off, Size - off) % (sizeof(vals) / sizeof(vals[0]))];
        off += 2;
    }
    else bps = 8;

    width = (width % 2048U) + 1U;
    height = (height % 2048U) + 1U;
    rowsperstrip = (Size >= off + 4) ? ((rd32(Data + off, Size - off) % height) + 1U) : height;
    if (Size >= off + 4) off += 4;

    photo = (Size > off) ? (uint16_t)(Data[off++] % 6) : 1;
    switch (photo)
    {
        case 0: photo = PHOTOMETRIC_MINISWHITE; break;
        case 1: photo = PHOTOMETRIC_MINISBLACK; break;
        case 2: photo = PHOTOMETRIC_RGB; break;
        case 3: photo = PHOTOMETRIC_PALETTE; break;
        case 4: photo = PHOTOMETRIC_SEPARATED; break;
        default: photo = PHOTOMETRIC_YCBCR; break;
    }

    config = (Size > off && (Data[off++] & 1)) ? EXTRASAMPLE_ASSOCALPHA : 0;
    planar = (Size > off && (Data[off++] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    orient = (Size > off) ? (uint16_t)((Data[off++] % 8) + 1) : ORIENTATION_TOPLEFT;
    use_tiles = (Size > off) ? (Data[off++] & 1) : 0;

    if (mode_sel != 4)
    {
        (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
        (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, orient);
        (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

        if (config != 0 && spp >= 2)
        {
            uint16_t extra_count = 1;
            uint16_t extra_types[1];
            extra_types[0] = config;
            (void)TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, extra_count, extra_types);
        }

        if (use_tiles)
        {
            uint32_t tw = 16, tl = 16;
            (void)TIFFSetField(tif, TIFFTAG_TILEWIDTH, tw);
            (void)TIFFSetField(tif, TIFFTAG_TILELENGTH, tl);
        }
        else
        {
            (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
        }
    }

    ret = TIFFWriteCheck(tif, use_tiles ? 1 : 0, "LLVMFuzzer");
    (void)ret;

    ret = TIFFFlushData(tif);
    (void)ret;

    ret = TIFFCheckpointDirectory(tif);
    (void)ret;

    ret = TIFFFlush(tif);
    (void)ret;

    ret = TIFFForceStrileArrayWriting(tif);
    (void)ret;

    ret = TIFFRewriteDirectory(tif);
    (void)ret;

    if (mode_sel != 4)
    {
        ret = TIFFWriteCheck(tif, use_tiles ? 0 : 1, "LLVMFuzzerAlt");
        (void)ret;

        ret = TIFFFlushData(tif);
        (void)ret;

        ret = TIFFCheckpointDirectory(tif);
        (void)ret;

        ret = TIFFFlush(tif);
        (void)ret;

        ret = TIFFForceStrileArrayWriting(tif);
        (void)ret;

        ret = TIFFRewriteDirectory(tif);
        (void)ret;
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}