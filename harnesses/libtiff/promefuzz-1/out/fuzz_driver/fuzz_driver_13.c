// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFDefaultTileSize at tif_tile.c:272:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFDefaultStripSize at tif_strip.c:210:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFSetupStrips at tif_write.c:523:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    typedef struct {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } MemFile;
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0) return 0;
    if (mf->pos > mf->size) return 0;
    size_t avail = mf->size - mf->pos;
    size_t want = (size_t)size;
    if (want > avail) want = avail;
    if (want > 0) {
        memcpy(buf, mf->data + mf->pos, want);
        mf->pos += want;
    }
    return (tmsize_t)want;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    (void)handle;
    (void)buf;
    if (size < 0) return 0;
    return size;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence) {
    typedef struct {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } MemFile;
    MemFile *mf = (MemFile *)handle;
    if (!mf) return (uint64_t)-1;

    size_t newpos = 0;
    if (whence == SEEK_SET) {
        newpos = (off > (uint64_t)SIZE_MAX) ? mf->size : (size_t)off;
    } else if (whence == SEEK_CUR) {
        uint64_t cur = (uint64_t)mf->pos;
        uint64_t sum = cur + off;
        newpos = (sum > (uint64_t)SIZE_MAX) ? mf->size : (size_t)sum;
    } else if (whence == SEEK_END) {
        uint64_t end = (uint64_t)mf->size;
        uint64_t sum = end + off;
        newpos = (sum > (uint64_t)SIZE_MAX) ? mf->size : (size_t)sum;
    } else {
        return (uint64_t)-1;
    }

    if (newpos > mf->size) newpos = mf->size;
    mf->pos = newpos;
    return (uint64_t)mf->pos;
}

static int mem_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static uint64_t mem_size(thandle_t handle) {
    typedef struct {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } MemFile;
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size) {
    typedef struct {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } MemFile;
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size) return 0;
    *base = (void *)mf->data;
    *size = (toff_t)mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    typedef struct {
        const uint8_t *data;
        size_t size;
        size_t pos;
    } MemFile;

    if (!Data) return 0;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    {
        MemFile mf;
        mf.data = Data;
        mf.size = Size;
        mf.pos = 0;

        const char *mode = "r";
        if (Size > 0) {
            switch (Data[0] % 4) {
                case 0: mode = "r"; break;
                case 1: mode = "rm"; break;
                case 2: mode = "rC"; break;
                default: mode = "rmc"; break;
            }
        }

        TIFF *tif = TIFFClientOpen(
            "mem",
            mode,
            (thandle_t)&mf,
            mem_read,
            mem_write,
            mem_seek,
            mem_close,
            mem_size,
            mem_map,
            mem_unmap);

        if (tif) {
            uint32_t w = 0, h = 0;
            TIFFDefaultTileSize(tif, &w, &h);
            (void)TIFFDefaultStripSize(tif, 0);
            (void)TIFFDefaultStripSize(tif, w);
            (void)TIFFDefaultStripSize(tif, h);
            (void)TIFFFileno(tif);
            (void)TIFFSetupStrips(tif);
            TIFFClose(tif);
        }
    }

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    {
        TIFF *tif = TIFFFdOpen(0, "./dummy_file", "r");
        if (tif) {
            uint32_t w = 1, h = 1;
            TIFFDefaultTileSize(tif, &w, &h);
            (void)TIFFDefaultStripSize(tif, 0);
            (void)TIFFDefaultStripSize(tif, (Size > 1) ? (uint32_t)Data[1] : 1U);
            (void)TIFFFileno(tif);
            (void)TIFFSetupStrips(tif);
            TIFFClose(tif);
        }
    }
#endif

    return 0;
}