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
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    char errbuf[1024];
    TIFF *tif = NULL;
    void *user_read_buf = NULL;
    void *decode_buf = NULL;
    void *tile_buf = NULL;
    tmsize_t user_read_buf_size = 0;
    tmsize_t decode_buf_size = 0;
    tmsize_t tile_buf_size = 0;

    write_dummy_file(Data, Size);

    tif = TIFFOpen("./dummy_file", "r");
    if (!tif) {
        return 0;
    }

    memset(errbuf, 0, sizeof(errbuf));
    (void)TIFFRGBAImageOK(tif, errbuf);

    user_read_buf_size = (tmsize_t)((Size > 0) ? (Size % 4096) : 0);
    if (user_read_buf_size > 0) {
        user_read_buf = malloc((size_t)user_read_buf_size);
        if (user_read_buf) {
            memcpy(user_read_buf, Data, (size_t)user_read_buf_size);
            (void)TIFFReadBufferSetup(tif, user_read_buf, user_read_buf_size);
        } else {
            (void)TIFFReadBufferSetup(tif, NULL, user_read_buf_size);
        }
    } else {
        (void)TIFFReadBufferSetup(tif, NULL, 0);
    }

    (void)TIFFSetupStrips(tif);

    decode_buf_size = (tmsize_t)((Size % 8192) + 1);
    decode_buf = malloc((size_t)decode_buf_size);
    if (decode_buf) {
        memset(decode_buf, 0, (size_t)decode_buf_size);
    }

    if (Size > 0 && decode_buf) {
        uint32_t strile = 0;
        if (Size >= 4) {
            strile = ((uint32_t)Data[0] << 24) |
                     ((uint32_t)Data[1] << 16) |
                     ((uint32_t)Data[2] << 8) |
                     (uint32_t)Data[3];
        }
        (void)TIFFReadFromUserBuffer(
            tif,
            strile,
            (void *)Data,
            (tmsize_t)Size,
            decode_buf,
            decode_buf_size);
    }

    tile_buf_size = (tmsize_t)((Size % 16384) + 1);
    tile_buf = malloc((size_t)tile_buf_size);
    if (tile_buf) {
        memset(tile_buf, 0, (size_t)tile_buf_size);
    }

    if (tile_buf) {
        uint32_t tile = 0;
        if (Size >= 8) {
            tile = ((uint32_t)Data[4] << 24) |
                   ((uint32_t)Data[5] << 16) |
                   ((uint32_t)Data[6] << 8) |
                   (uint32_t)Data[7];
        }
        (void)TIFFReadEncodedTile(tif, tile, tile_buf, tile_buf_size);
        (void)TIFFReadEncodedTile(tif, 0, tile_buf, tile_buf_size);
    }

    for (int i = 0; i < 4; ++i) {
        if (!TIFFReadDirectory(tif)) {
            break;
        }

        memset(errbuf, 0, sizeof(errbuf));
        (void)TIFFRGBAImageOK(tif, errbuf);
        (void)TIFFSetupStrips(tif);

        if (decode_buf && Size > 0) {
            uint32_t strile = (uint32_t)i;
            (void)TIFFReadFromUserBuffer(
                tif,
                strile,
                (void *)Data,
                (tmsize_t)Size,
                decode_buf,
                decode_buf_size);
        }

        if (tile_buf) {
            (void)TIFFReadEncodedTile(tif, (uint32_t)i, tile_buf, tile_buf_size);
        }
    }

    TIFFClose(tif);
    free(tile_buf);
    free(decode_buf);
    free(user_read_buf);
    return 0;
}