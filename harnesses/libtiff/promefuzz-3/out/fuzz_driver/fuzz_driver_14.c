// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadDirectory at tif_dirread.c:3580:1 in tiffio.h
// TIFFNumberOfStrips at tif_strip.c:59:1 in tiffio.h
// TIFFStripSize at tif_strip.c:195:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFVStripSize at tif_strip.c:128:1 in tiffio.h
// TIFFRawStripSize at tif_strip.c:157:1 in tiffio.h
// TIFFReadEncodedStrip at tif_read.c:504:1 in tiffio.h
// TIFFReadRawStrip at tif_read.c:682:1 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFReadFromUserBuffer at tif_read.c:1433:10 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16le(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32le(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void wr16le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
}

static void wr32le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
    p[2] = (uint8_t)((v >> 16) & 0xff);
    p[3] = (uint8_t)((v >> 24) & 0xff);
}

static size_t build_minimal_tiff(const uint8_t *Data, size_t Size, uint8_t **out) {
    const uint32_t width = (Size > 0) ? ((uint32_t)Data[0] + 1U) : 1U;
    const uint32_t height = (Size > 1) ? ((uint32_t)Data[1] + 1U) : 1U;
    const uint32_t rowsperstrip = (Size > 2) ? (((uint32_t)Data[2] % height) + 1U) : 1U;
    const uint32_t payload_len = (Size > 3) ? (uint32_t)(((Size - 3) > 4096) ? 4096 : (Size - 3)) : 1U;

    const uint16_t entry_count = 9;
    const uint32_t ifd_offset = 8;
    const uint32_t ifd_size = 2 + (uint32_t)entry_count * 12 + 4;
    const uint32_t strip_offset = ifd_offset + ifd_size;
    const uint32_t total_size = strip_offset + payload_len;

    uint8_t *buf = (uint8_t *)malloc(total_size);
    uint8_t *p;
    uint32_t i;

    (void)rd16le;
    (void)rd32le;

    if (!buf)
        return 0;

    memset(buf, 0, total_size);

    buf[0] = 'I';
    buf[1] = 'I';
    wr16le(buf + 2, 42);
    wr32le(buf + 4, ifd_offset);

    p = buf + ifd_offset;
    wr16le(p, entry_count);
    p += 2;

    wr16le(p + 0, 256); wr16le(p + 2, 4); wr32le(p + 4, 1); wr32le(p + 8, width); p += 12;
    wr16le(p + 0, 257); wr16le(p + 2, 4); wr32le(p + 4, 1); wr32le(p + 8, height); p += 12;
    wr16le(p + 0, 258); wr16le(p + 2, 3); wr32le(p + 4, 1); wr32le(p + 8, 8); p += 12;
    wr16le(p + 0, 259); wr16le(p + 2, 3); wr32le(p + 4, 1); wr32le(p + 8, 1); p += 12;
    wr16le(p + 0, 262); wr16le(p + 2, 3); wr32le(p + 4, 1); wr32le(p + 8, 1); p += 12;
    wr16le(p + 0, 273); wr16le(p + 2, 4); wr32le(p + 4, 1); wr32le(p + 8, strip_offset); p += 12;
    wr16le(p + 0, 277); wr16le(p + 2, 3); wr32le(p + 4, 1); wr32le(p + 8, 1); p += 12;
    wr16le(p + 0, 278); wr16le(p + 2, 4); wr32le(p + 4, 1); wr32le(p + 8, rowsperstrip); p += 12;
    wr16le(p + 0, 279); wr16le(p + 2, 4); wr32le(p + 4, 1); wr32le(p + 8, payload_len); p += 12;

    wr32le(p, 0);

    if (Size > 3) {
        memcpy(buf + strip_offset, Data + 3, payload_len);
    } else {
        for (i = 0; i < payload_len; i++)
            buf[strip_offset + i] = 0;
    }

    *out = buf;
    return total_size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif;
    uint8_t *filebuf = NULL;
    size_t file_size = 0;
    FILE *fp;
    uint32_t nstrips = 0;
    tmsize_t strip_size = 0;
    tmsize_t vstrip1 = 0, vstrip2 = 0;
    uint32_t strip;
    uint8_t *buf1 = NULL, *buf2 = NULL, *user_in = NULL, *user_out = NULL;
    tmsize_t r1, r2;
    size_t alloc1 = 0, alloc2 = 0, in_sz = 0, out_sz = 0;

    file_size = build_minimal_tiff(Data, Size, &filebuf);
    if (file_size == 0 || filebuf == NULL)
        return 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp) {
        free(filebuf);
        return 0;
    }
    fwrite(filebuf, 1, file_size, fp);
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        free(filebuf);
        return 0;
    }

    (void)TIFFReadDirectory(tif);

    nstrips = TIFFNumberOfStrips(tif);
    strip_size = TIFFStripSize(tif);
    vstrip1 = TIFFVStripSize(tif, 1);
    vstrip2 = TIFFVStripSize(tif, (Size > 4) ? (uint32_t)Data[4] : 0);

    if (strip_size < 0) strip_size = 0;
    if (vstrip1 < 0) vstrip1 = 0;
    if (vstrip2 < 0) vstrip2 = 0;

    alloc1 = (size_t)((strip_size > 0 && strip_size < (tmsize_t)(1 << 20)) ? strip_size : 4096);
    if (alloc1 == 0)
        alloc1 = 1;

    buf1 = (uint8_t *)malloc(alloc1);
    if (!buf1)
        goto done;
    memset(buf1, 0, alloc1);

    for (strip = 0; strip < (nstrips ? nstrips : 1U) && strip < 4U; strip++) {
        tmsize_t req1 = (Size > 5 && (Data[5] & 1)) ? (tmsize_t)-1 : (tmsize_t)alloc1;
        tmsize_t raw_needed = TIFFRawStripSize(tif, strip);
        tmsize_t req2_local;

        r1 = TIFFReadEncodedStrip(tif, strip, buf1, req1);
        (void)r1;

        if (raw_needed < 0)
            raw_needed = 0;

        alloc2 = (size_t)((raw_needed > 0 && raw_needed < (tmsize_t)(1 << 20)) ? raw_needed : 4096);
        if (alloc2 == 0)
            alloc2 = 1;

        free(buf2);
        buf2 = (uint8_t *)malloc(alloc2);
        if (!buf2)
            goto done;
        memset(buf2, 0, alloc2);

        if (Size > 6 && (Data[6] & 1))
            req2_local = (tmsize_t)-1;
        else
            req2_local = raw_needed;

        if (req2_local < 0 || (size_t)req2_local > alloc2)
            req2_local = (tmsize_t)alloc2;

        r2 = TIFFReadRawStrip(tif, strip, buf2, req2_local);
        (void)r2;
    }

    in_sz = (Size > 7) ? (size_t)(Data[7] % 128) : 0;
    out_sz = (Size > 8) ? (size_t)(Data[8] % 512) : 0;
    if (in_sz == 0)
        in_sz = alloc1 < 64 ? alloc1 : 64;
    if (out_sz == 0) {
        size_t fallback = (vstrip2 > 0 && vstrip2 < (tmsize_t)(1 << 20)) ? (size_t)vstrip2 :
                          ((vstrip1 > 0 && vstrip1 < (tmsize_t)(1 << 20)) ? (size_t)vstrip1 : 256);
        out_sz = fallback ? fallback : 1;
    }

    user_in = (uint8_t *)malloc(in_sz);
    user_out = (uint8_t *)malloc(out_sz);
    if (user_in && user_out) {
        size_t copy_in = 0;
        uint32_t target_strip = 0;

        memset(user_in, 0, in_sz);
        memset(user_out, 0, out_sz);

        if (Size > 9) {
            copy_in = Size - 9;
            if (copy_in > in_sz)
                copy_in = in_sz;
            memcpy(user_in, Data + 9, copy_in);
        }

        if (nstrips > 0 && Size > 10)
            target_strip = ((uint32_t)Data[10]) % nstrips;

        (void)TIFFReadFromUserBuffer(tif, target_strip, user_in, (tmsize_t)in_sz, user_out, (tmsize_t)out_sz);

        if (nstrips > 0)
            (void)TIFFReadFromUserBuffer(tif, nstrips - 1, user_in, (tmsize_t)in_sz, user_out, (tmsize_t)out_sz);

        (void)TIFFReadFromUserBuffer(tif, nstrips + 1U, user_in, (tmsize_t)in_sz, user_out, (tmsize_t)out_sz);
    }

done:
    free(user_out);
    free(user_in);
    free(buf2);
    free(buf1);
    TIFFClose(tif);
    free(filebuf);
    return 0;
}