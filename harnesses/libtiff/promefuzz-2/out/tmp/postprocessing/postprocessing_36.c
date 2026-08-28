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

static void write_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static uint32_t pick_u32(const uint8_t *Data, size_t Size, size_t off) {
    uint32_t v = 0;
    if (Size > off) v |= (uint32_t)Data[off];
    if (Size > off + 1) v |= ((uint32_t)Data[off + 1]) << 8;
    if (Size > off + 2) v |= ((uint32_t)Data[off + 2]) << 16;
    if (Size > off + 3) v |= ((uint32_t)Data[off + 3]) << 24;
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        tmsize_t read_buf_size = 0;
        if (Size > 0) {
            read_buf_size = (tmsize_t)((pick_u32(Data, Size, 0) % (Size + 1)) + 1);
        } else {
            read_buf_size = 1;
        }

        uint8_t *read_buf = (uint8_t *)malloc((size_t)read_buf_size);
        if (read_buf) {
            memset(read_buf, 0, (size_t)read_buf_size);
            (void)TIFFReadBufferSetup(tif, read_buf, read_buf_size);
        } else {
            (void)TIFFReadBufferSetup(tif, NULL, read_buf_size);
        }

        int err = 0;
        uint32_t strile = pick_u32(Data, Size, 4);
        (void)TIFFGetStrileOffsetWithErr(tif, strile, &err);
        (void)TIFFGetStrileOffsetWithErr(tif, 0, NULL);

        tmsize_t out_size1 = 0;
        if (Size > 0) {
            out_size1 = (tmsize_t)((pick_u32(Data, Size, 8) % (Size + 1)) + 1);
        } else {
            out_size1 = 1;
        }

        uint8_t *out1 = (uint8_t *)malloc((size_t)out_size1);
        if (out1) {
            memset(out1, 0, (size_t)out_size1);
            (void)TIFFReadEncodedStrip(tif, strile, out1, out_size1);
            (void)TIFFReadEncodedStrip(tif, 0, out1, (tmsize_t)-1);
            (void)TIFFReadEncodedTile(tif, strile, out1, out_size1);
            (void)TIFFReadEncodedTile(tif, 0, out1, (tmsize_t)-1);
        }

        if (Size > 0 && out1) {
            tmsize_t in_size = (tmsize_t)Size;
            (void)TIFFReadFromUserBuffer(tif, strile, (void *)Data, in_size, out1, out_size1);
            (void)TIFFReadFromUserBuffer(tif, 0, (void *)Data, in_size, out1, out_size1);
        }

        free(out1);
        free(read_buf);
        TIFFClose(tif);
    }

    TIFF *tifw = TIFFOpen("./dummy_file", "w");
    if (tifw) {
        tmsize_t write_buf_size = -1;
        if (Size > 0) {
            uint32_t selector = pick_u32(Data, Size, 12) % 3U;
            if (selector == 0) {
                write_buf_size = -1;
                (void)TIFFWriteBufferSetup(tifw, NULL, write_buf_size);
            } else {
                write_buf_size = (tmsize_t)((pick_u32(Data, Size, 16) % (Size + 1)) + 1);
                if (selector == 1) {
                    uint8_t *wbuf = (uint8_t *)malloc((size_t)write_buf_size);
                    if (wbuf) {
                        memset(wbuf, 0, (size_t)write_buf_size);
                        (void)TIFFWriteBufferSetup(tifw, wbuf, write_buf_size);
                        free(wbuf);
                    } else {
                        (void)TIFFWriteBufferSetup(tifw, NULL, write_buf_size);
                    }
                } else {
                    (void)TIFFWriteBufferSetup(tifw, NULL, write_buf_size);
                }
            }
        } else {
            (void)TIFFWriteBufferSetup(tifw, NULL, -1);
        }
        TIFFClose(tifw);
    }

    return 0;
}