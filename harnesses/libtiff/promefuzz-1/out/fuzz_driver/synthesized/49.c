// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFReadGPSDirectory at tif_dirread.c:4560:1 in tiffio.h
// TIFFReadEXIFDirectory at tif_dirread.c:4549:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFFieldName at tif_dirinfo.c:762:1 in tiffio.h
// TIFFWarning at tif_warning.c:49:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
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

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return;
    }
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

int LLVMFuzzerTestOneInput_49(const uint8_t *Data, size_t Size) {
    write_input_file(Data, Size);

    TIFFWarning("fuzz", "size=%zu", Size);
    if (Size > 0) {
        TIFFWarning("fuzz", "first_byte=%u", (unsigned)Data[0]);
    }

    TIFF *tif = TIFFOpen("./dummy_file", "r");
    if (tif) {
        toff_t off1 = 0;
        toff_t off2 = 0;
        toff_t off3 = 0;

        if (Size >= 8) {
            memcpy(&off1, Data, 8);
        } else {
            off1 = (toff_t)Size;
        }

        if (Size >= 16) {
            memcpy(&off2, Data + 8, 8);
        } else {
            off2 = off1 ^ (toff_t)0xFFFFFFFFFFFFFFFFULL;
        }

        off3 = (toff_t)(Size > 0 ? Data[Size / 2] : 0);

        (void)TIFFReadGPSDirectory(tif, off1);
        (void)TIFFReadEXIFDirectory(tif, off1);
        (void)TIFFReadGPSDirectory(tif, off2);
        (void)TIFFReadEXIFDirectory(tif, off2);
        (void)TIFFReadGPSDirectory(tif, off3);
        (void)TIFFReadEXIFDirectory(tif, off3);

        {
            uint32_t tag = 0;
            TIFFDataType dtype = TIFF_ANY;

            if (Size >= 4) {
                memcpy(&tag, Data, 4);
            } else {
                tag = (uint32_t)Size;
            }

            if (Size > 4) {
                switch (Data[4] % 8) {
                    case 0: dtype = TIFF_ANY; break;
                    case 1: dtype = TIFF_BYTE; break;
                    case 2: dtype = TIFF_ASCII; break;
                    case 3: dtype = TIFF_SHORT; break;
                    case 4: dtype = TIFF_LONG; break;
                    case 5: dtype = TIFF_RATIONAL; break;
                    case 6: dtype = TIFF_IFD; break;
                    default: dtype = TIFF_UNDEFINED; break;
                }
            }

            const TIFFField *field = TIFFFindField(tif, tag, dtype);
            if (field) {
                const char *name = TIFFFieldName(field);
                if (name) {
                    TIFFWarning("fuzz", "field=%s", name);
                }
            }
        }

        TIFFClose(tif);
    }

    tif = TIFFOpen("./dummy_file", "r+");
    if (tif) {
        (void)TIFFCreateGPSDirectory(tif);

        {
            uint32_t tag = 0x8825U;
            const TIFFField *field = TIFFFindField(tif, tag, TIFF_ANY);
            if (field) {
                const char *name = TIFFFieldName(field);
                if (name) {
                    TIFFWarning("fuzz", "post-create field=%s", name);
                }
            }
        }

        if (Size >= 8) {
            toff_t off;
            memcpy(&off, Data, 8);
            (void)TIFFReadGPSDirectory(tif, off);
            (void)TIFFReadEXIFDirectory(tif, off);
        } else {
            (void)TIFFReadGPSDirectory(tif, 0);
            (void)TIFFReadEXIFDirectory(tif, 0);
        }

        TIFFClose(tif);
    }

    return 0;
}