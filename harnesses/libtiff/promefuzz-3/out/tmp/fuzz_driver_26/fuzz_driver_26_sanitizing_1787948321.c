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

static void fuzz_tiff_handle(TIFF *tif, const uint8_t *Data, size_t Size) {
    char errbuf[1024];
    (void)memset(errbuf, 0, sizeof(errbuf));

    (void)TIFFGetMode(tif);
    (void)TIFFIsMSB2LSB(tif);
    (void)TIFFRGBAImageOK(tif, errbuf);

    for (int i = 0; i < 4; ++i) {
        if (!TIFFReadDirectory(tif))
            break;

        (void)TIFFGetMode(tif);
        (void)TIFFIsMSB2LSB(tif);
        (void)memset(errbuf, 0, sizeof(errbuf));
        (void)TIFFRGBAImageOK(tif, errbuf);
    }

    uint32_t tilew = 0, tileh = 0;
    if (TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilew) &&
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileh) &&
        tilew > 0 && tileh > 0) {
        size_t npixels = (size_t)tilew * (size_t)tileh;
        if (npixels > 0 && npixels <= (SIZE_MAX / sizeof(uint32_t))) {
            uint32_t *raster = (uint32_t *)malloc(npixels * sizeof(uint32_t));
            if (raster) {
                uint32_t cols[4] = {0, tilew, tilew > 1 ? tilew - 1 : 0, 1};
                uint32_t rows[4] = {0, tileh, tileh > 1 ? tileh - 1 : 0, 1};
                int stops[2] = {0, 1};

                for (size_t ci = 0; ci < 4; ++ci) {
                    for (size_t ri = 0; ri < 4; ++ri) {
                        for (size_t si = 0; si < 2; ++si) {
                            (void)TIFFReadRGBATileExt(tif, cols[ci], rows[ri], raster, stops[si]);
                        }
                    }
                }
                free(raster);
            }
        }
    }

    if (Size > 0) {
        tmsize_t outsize = 0;
        tmsize_t insize = (tmsize_t)Size;
        if (insize > 0) {
            outsize = (tmsize_t)(Size * 4 + 64);
            if (outsize < 0) outsize = 0;
        }

        if (outsize > 0) {
            void *outbuf = malloc((size_t)outsize);
            if (outbuf) {
                uint32_t striles[] = {0, 1, (uint32_t)(Size & 0xFFFFu), 0xFFFFFFFFu};
                for (size_t i = 0; i < sizeof(striles) / sizeof(striles[0]); ++i) {
                    (void)TIFFReadFromUserBuffer(
                        tif, striles[i], (void *)Data, insize, outbuf, outsize);
                }
                free(outbuf);
            }
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    write_dummy_file(Data, Size);

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        fuzz_tiff_handle(tif, Data, Size);
        TIFFClose(tif);
    }

    TIFF *tif_header = TIFFOpen("./dummy_file", "rh");
    if (tif_header) {
        fuzz_tiff_handle(tif_header, Data, Size);
        TIFFClose(tif_header);
    }

    return 0;
}