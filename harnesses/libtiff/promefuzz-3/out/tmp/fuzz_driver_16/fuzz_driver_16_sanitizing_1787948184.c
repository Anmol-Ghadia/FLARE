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
#include <tiffio.h>

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t pos;
} MemFile;

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0) return 0;
    if (mf->pos > mf->size) return 0;
    size_t avail = mf->size - mf->pos;
    size_t want = (size_t)size;
    if (want > avail) want = avail;
    if (want > 0) {
        memcpy(buf, mf->buf + mf->pos, want);
        mf->pos += want;
    }
    return (tmsize_t)want;
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || size < 0) return 0;
    if (!buf && size > 0) return 0;

    size_t want = (size_t)size;
    if (mf->pos > mf->size) return 0;

    if (want > mf->size - mf->pos) {
        size_t newsize = mf->pos + want;
        uint8_t *newbuf = (uint8_t *)realloc(mf->buf, newsize ? newsize : 1);
        if (!newbuf) {
            size_t avail = mf->size - mf->pos;
            if (avail > 0 && buf) {
                memcpy(mf->buf + mf->pos, buf, avail);
                mf->pos += avail;
            }
            return (tmsize_t)avail;
        }
        if (newsize > mf->size) {
            memset(newbuf + mf->size, 0, newsize - mf->size);
        }
        mf->buf = newbuf;
        mf->size = newsize;
    }

    if (want > 0 && buf) {
        memcpy(mf->buf + mf->pos, buf, want);
        mf->pos += want;
    }
    return (tmsize_t)want;
}

static uint64_t fuzz_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return (uint64_t)-1;

    size_t base = 0;
    switch (whence) {
        case SEEK_SET: base = 0; break;
        case SEEK_CUR: base = mf->pos; break;
        case SEEK_END: base = mf->size; break;
        default: return (uint64_t)-1;
    }

    if (off > (uint64_t)SIZE_MAX) return (uint64_t)-1;
    if (base > SIZE_MAX - (size_t)off) return (uint64_t)-1;

    mf->pos = base + (size_t)off;
    return (uint64_t)mf->pos;
}

static int fuzz_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static uint64_t fuzz_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    return (uint64_t)mf->size;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size) return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void exercise_tiff(TIFF *tif, MemFile *mf, const uint8_t *Data, size_t Size) {
    if (!tif) return;

    TIFFReadWriteProc rp = TIFFGetReadProc(tif);
    TIFFReadWriteProc wp = TIFFGetWriteProc(tif);
    TIFFUnmapFileProc up = TIFFGetUnmapFileProc(tif);
    TIFFCloseProc cp = TIFFGetCloseProc(tif);

    if (rp) {
        uint8_t tmp[32];
        tmsize_t amt = (Size > 0) ? (tmsize_t)(Data[0] % sizeof(tmp)) : 0;
        (void)rp((thandle_t)mf, tmp, amt);
    }

    if (wp) {
        uint8_t tmp[32];
        size_t n = (Size > 1) ? (size_t)(Data[1] % sizeof(tmp)) : 0;
        if (n > 0) memcpy(tmp, Data, n <= Size ? n : Size);
        (void)wp((thandle_t)mf, tmp, (tmsize_t)n);
    }

    if (up) {
        void *base = mf ? (void *)mf->buf : NULL;
        toff_t sz = mf ? (toff_t)mf->size : 0;
        up((thandle_t)mf, base, sz);
    }

    if (cp) {
        (void)cp((thandle_t)mf);
    }

    (void)TIFFSetClientdata(tif, (thandle_t)mf);
    (void)TIFFSetClientdata(tif, (thandle_t)Data);
    (void)TIFFSetClientdata(tif, NULL);
    (void)TIFFSetClientdata(tif, (thandle_t)mf);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    MemFile mf;
    mf.size = Size;
    mf.pos = 0;
    mf.buf = NULL;

    if (Size > 0) {
        mf.buf = (uint8_t *)malloc(Size);
        if (!mf.buf) return 0;
        memcpy(mf.buf, Data, Size);
    }

    {
        TIFF *tif = TIFFClientOpen(
            "mem-r",
            "r",
            (thandle_t)&mf,
            fuzz_read,
            fuzz_write,
            fuzz_seek,
            fuzz_close,
            fuzz_size,
            fuzz_map,
            fuzz_unmap);
        if (tif) {
            exercise_tiff(tif, &mf, Data, Size);
            TIFFClose(tif);
        }
    }

    mf.pos = 0;
    {
        TIFF *tif = TIFFClientOpen(
            "mem-rw",
            "w+",
            (thandle_t)&mf,
            fuzz_read,
            fuzz_write,
            fuzz_seek,
            fuzz_close,
            fuzz_size,
            fuzz_map,
            fuzz_unmap);
        if (tif) {
            exercise_tiff(tif, &mf, Data, Size);
            TIFFClose(tif);
        }
    }

    mf.pos = 0;
    {
        TIFF *tif = TIFFClientOpen(
            "mem-a",
            "a+",
            (thandle_t)&mf,
            fuzz_read,
            fuzz_write,
            fuzz_seek,
            fuzz_close,
            fuzz_size,
            fuzz_map,
            fuzz_unmap);
        if (tif) {
            exercise_tiff(tif, &mf, Data, Size);
            TIFFClose(tif);
        }
    }

    free(mf.buf);
    return 0;
}