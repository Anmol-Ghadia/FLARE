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

static void write_dummy_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint32_t get_u32(const uint8_t *Data, size_t Size, size_t off) {
    uint32_t v = 0;
    if (off < Size) v |= (uint32_t)Data[off];
    if (off + 1 < Size) v |= ((uint32_t)Data[off + 1]) << 8;
    if (off + 2 < Size) v |= ((uint32_t)Data[off + 2]) << 16;
    if (off + 3 < Size) v |= ((uint32_t)Data[off + 3]) << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        uint32_t idx1 = get_u32(Data, Size, 0);
        uint32_t idx2 = get_u32(Data, Size, 4);
        uint32_t idx3 = get_u32(Data, Size, 8);

        (void)TIFFCurrentTile(tif);
        (void)TIFFRawStripSize64(tif, idx1);
        (void)TIFFGetStrileByteCount(tif, idx2);

        {
            int err = 0;
            (void)TIFFGetStrileByteCountWithErr(tif, idx3, &err);
        }

        (void)TIFFGetStrileOffset(tif, idx1);

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "w");
    if (tif) {
        uint32_t width = (Size > 0 ? (uint32_t)(Data[0] % 32) + 1 : 1);
        uint32_t height = (Size > 1 ? (uint32_t)(Data[1] % 32) + 1 : 1);
        uint16_t spp = (Size > 2 && (Data[2] & 1)) ? 3 : 1;
        uint16_t bps = 8;
        uint16_t photo = (spp == 1) ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;
        uint32_t rowsperstrip = (Size > 3 ? (uint32_t)(Data[3] % height) + 1 : 1);

        (void)TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
        (void)TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
        (void)TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
        (void)TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
        (void)TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        (void)TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        (void)TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
        (void)TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        (void)TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

        {
            tmsize_t linebytes = (tmsize_t)width * spp * (bps / 8);
            tmsize_t bufsize = linebytes * rowsperstrip;
            if (bufsize < 0) bufsize = 0;

            if (bufsize > 0) {
                unsigned char stackbuf[4096];
                void *buf = stackbuf;
                tmsize_t use_size = bufsize;
                if ((size_t)use_size > sizeof(stackbuf)) {
                    use_size = (tmsize_t)sizeof(stackbuf);
                }

                if (Size > 0) {
                    for (tmsize_t i = 0; i < use_size; ++i) {
                        ((unsigned char *)buf)[i] = Data[(size_t)i % Size];
                    }
                } else {
                    memset(buf, 0, (size_t)use_size);
                }

                (void)TIFFWriteEncodedStrip(tif, 0, buf, use_size);
                (void)TIFFWriteEncodedStrip(tif, 1, buf, use_size / 2);
                (void)TIFFWriteEncodedStrip(tif, get_u32(Data, Size, 12), buf, use_size);

                (void)TIFFCurrentTile(tif);
                (void)TIFFRawStripSize64(tif, 0);
                (void)TIFFGetStrileByteCount(tif, 0);
                {
                    int err = 0;
                    (void)TIFFGetStrileByteCountWithErr(tif, 0, &err);
                }
                (void)TIFFGetStrileOffset(tif, 0);
            }
        }

        TIFFClose(tif);
    }

    return 0;
}