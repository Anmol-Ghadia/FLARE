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

static void write_u16_le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
}

static void write_u32_le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
    p[2] = (uint8_t)((v >> 16) & 0xff);
    p[3] = (uint8_t)((v >> 24) & 0xff);
}

static void build_minimal_tiff(const uint8_t *Data, size_t Size) {
    uint8_t buf[256];
    memset(buf, 0, sizeof(buf));

    /* TIFF header: little-endian classic TIFF */
    buf[0] = 'I';
    buf[1] = 'I';
    write_u16_le(buf + 2, 42);
    write_u32_le(buf + 4, 8); /* first IFD at offset 8 */

    /* 9 directory entries */
    write_u16_le(buf + 8, 9);

    /* Entry layout starts at offset 10, 12 bytes each */
    size_t e = 10;
    uint32_t width = (Size > 0 ? (uint32_t)(Data[0] % 8 + 1) : 1);
    uint32_t height = (Size > 1 ? (uint32_t)(Data[1] % 8 + 1) : 1);
    uint32_t rowsperstrip = (Size > 2 ? (uint32_t)(Data[2] % 8 + 1) : 1);
    if (rowsperstrip > height) rowsperstrip = height;
    if (rowsperstrip == 0) rowsperstrip = 1;
    uint32_t imagelen = width * height;

    /* ImageWidth (256), LONG, 1 */
    write_u16_le(buf + e + 0, 256);
    write_u16_le(buf + e + 2, 4);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, width);
    e += 12;

    /* ImageLength (257), LONG, 1 */
    write_u16_le(buf + e + 0, 257);
    write_u16_le(buf + e + 2, 4);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, height);
    e += 12;

    /* BitsPerSample (258), SHORT, 1 => 8 */
    write_u16_le(buf + e + 0, 258);
    write_u16_le(buf + e + 2, 3);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, 8);
    e += 12;

    /* Compression (259), SHORT, 1 => 1 */
    write_u16_le(buf + e + 0, 259);
    write_u16_le(buf + e + 2, 3);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, 1);
    e += 12;

    /* PhotometricInterpretation (262), SHORT, 1 => 1 */
    write_u16_le(buf + e + 0, 262);
    write_u16_le(buf + e + 2, 3);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, 1);
    e += 12;

    /* StripOffsets (273), LONG, 1 => pixel data offset */
    write_u16_le(buf + e + 0, 273);
    write_u16_le(buf + e + 2, 4);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, 122);
    e += 12;

    /* SamplesPerPixel (277), SHORT, 1 => 1 */
    write_u16_le(buf + e + 0, 277);
    write_u16_le(buf + e + 2, 3);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, 1);
    e += 12;

    /* RowsPerStrip (278), LONG, 1 */
    write_u16_le(buf + e + 0, 278);
    write_u16_le(buf + e + 2, 4);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, rowsperstrip);
    e += 12;

    /* StripByteCounts (279), LONG, 1 */
    write_u16_le(buf + e + 0, 279);
    write_u16_le(buf + e + 2, 4);
    write_u32_le(buf + e + 4, 1);
    write_u32_le(buf + e + 8, imagelen);
    e += 12;

    /* next IFD offset = 0 */
    write_u32_le(buf + 8 + 2 + 9 * 12, 0);

    /* Pixel data at offset 122 */
    for (uint32_t i = 0; i < imagelen && 122 + i < sizeof(buf); i++) {
        buf[122 + i] = (Size > 3 + i) ? Data[3 + i] : (uint8_t)i;
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    fwrite(buf, 1, sizeof(buf), fp);
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    build_minimal_tiff(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (!tif)
        return 0;

    uint32_t idxs[8];
    idxs[0] = 0;
    idxs[1] = 1;
    idxs[2] = 2;
    idxs[3] = 0xFFFFFFFFu;
    idxs[4] = (Size >= 4) ? ((uint32_t)Data[0] |
                             ((uint32_t)Data[1] << 8) |
                             ((uint32_t)Data[2] << 16) |
                             ((uint32_t)Data[3] << 24))
                          : 3;
    idxs[5] = (Size > 0) ? Data[0] : 4;
    idxs[6] = (Size > 1) ? ((uint32_t)Data[1] << 1) : 5;
    idxs[7] = (Size > 2) ? ((uint32_t)Data[2] << 8) : 6;

    for (size_t i = 0; i < sizeof(idxs) / sizeof(idxs[0]); i++) {
        int err1 = -1;
        int err2 = -1;
        volatile uint64_t a = TIFFGetStrileOffset(tif, idxs[i]);
        volatile uint64_t b = TIFFGetStrileByteCount(tif, idxs[i]);
        volatile uint64_t c = TIFFRawStripSize64(tif, idxs[i]);
        volatile uint64_t d = TIFFGetStrileOffsetWithErr(tif, idxs[i], &err1);
        volatile uint64_t e = TIFFGetStrileByteCountWithErr(tif, idxs[i], &err2);
        volatile uint64_t f = TIFFTileSize64(tif);
        (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
        (void)err1; (void)err2;
    }

    /* Exercise optional NULL error pointers too */
    (void)TIFFGetStrileOffsetWithErr(tif, 0, NULL);
    (void)TIFFGetStrileByteCountWithErr(tif, 0, NULL);
    (void)TIFFTileSize64(tif);

    TIFFClose(tif);
    return 0;
}