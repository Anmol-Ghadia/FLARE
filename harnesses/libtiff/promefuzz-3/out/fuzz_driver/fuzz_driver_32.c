// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFGetReadProc at tif_open.c:676:1 in tiffio.h
// TIFFGetCloseProc at tif_open.c:703:1 in tiffio.h
// TIFFGetSizeProc at tif_open.c:712:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFGetReadProc at tif_open.c:676:1 in tiffio.h
// TIFFGetCloseProc at tif_open.c:703:1 in tiffio.h
// TIFFGetSizeProc at tif_open.c:712:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t pos;
    int closed;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0 || mf->closed) return 0;
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

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    (void)buf;
    if (!mf || size < 0 || mf->closed) return 0;
    if (mf->pos > mf->size) return 0;
    size_t want = (size_t)size;
    if (want > mf->size - mf->pos) want = mf->size - mf->pos;
    mf->pos += want;
    return (tmsize_t)want;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || mf->closed) return (uint64_t)-1;

    uint64_t newpos = 0;
    if (whence == SEEK_SET) {
        newpos = off;
    } else if (whence == SEEK_CUR) {
        newpos = (uint64_t)mf->pos + off;
    } else if (whence == SEEK_END) {
        newpos = (uint64_t)mf->size + off;
    } else {
        return (uint64_t)-1;
    }

    if (newpos > mf->size) {
        mf->pos = mf->size;
    } else {
        mf->pos = (size_t)newpos;
    }
    return (uint64_t)mf->pos;
}

static int mem_close(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    mf->closed = 1;
    return 0;
}

static uint64_t mem_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size || mf->closed) return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static tmsize_t zero_read(thandle_t handle, void *buf, tmsize_t size) {
    (void)handle;
    if (buf && size > 0) memset(buf, 0, (size_t)size);
    return size > 0 ? size : 0;
}

static tmsize_t zero_write(thandle_t handle, void *buf, tmsize_t size) {
    (void)handle;
    (void)buf;
    return size > 0 ? size : 0;
}

static uint64_t zero_seek(thandle_t handle, uint64_t off, int whence) {
    (void)handle;
    (void)whence;
    return off;
}

static int zero_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static uint64_t zero_size(thandle_t handle) {
    (void)handle;
    return 0;
}

static int zero_map(thandle_t handle, void **base, toff_t *size) {
    (void)handle;
    if (base) *base = NULL;
    if (size) *size = 0;
    return 0;
}

static void zero_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    uint8_t *copy = NULL;
    if (Size > 0) {
        copy = (uint8_t *)malloc(Size);
        if (!copy) return 0;
        memcpy(copy, Data, Size);
    }

    MemFile mf;
    mf.buf = copy;
    mf.size = Size;
    mf.pos = 0;
    mf.closed = 0;

    const char *modes[] = { "r", "rm", "rM", "w", "w8", "a", "r+" };
    size_t mode_index = (Size > 0) ? (Data[0] % (sizeof(modes) / sizeof(modes[0]))) : 0;

    TIFF *tif1 = TIFFClientOpen(
        "mem1",
        modes[mode_index],
        (thandle_t)&mf,
        mem_read,
        mem_write,
        mem_seek,
        mem_close,
        mem_size,
        mem_map,
        mem_unmap
    );

    if (tif1) {
        TIFFReadWriteProc rp = TIFFGetReadProc(tif1);
        TIFFCloseProc cp = TIFFGetCloseProc(tif1);
        TIFFSizeProc sp = TIFFGetSizeProc(tif1);
        thandle_t cd1 = TIFFClientdata(tif1);
        thandle_t old1 = TIFFSetClientdata(tif1, (thandle_t)copy);
        thandle_t cd2 = TIFFClientdata(tif1);
        thandle_t old2 = TIFFSetClientdata(tif1, old1);
        thandle_t cd3 = TIFFClientdata(tif1);

        if (rp) {
            uint8_t tmp[32];
            tmsize_t req = (Size > 1) ? (tmsize_t)(Data[1] % sizeof(tmp)) : (tmsize_t)sizeof(tmp);
            rp(TIFFClientdata(tif1), tmp, req);
        }
        if (sp) {
            (void)sp(TIFFClientdata(tif1));
        }
        (void)cp;
        (void)cd1;
        (void)cd2;
        (void)cd3;
        (void)old2;

        TIFFClose(tif1);
    }

    MemFile mf2;
    mf2.buf = copy;
    mf2.size = Size;
    mf2.pos = 0;
    mf2.closed = 0;

    TIFF *tif2 = TIFFClientOpen(
        "mem2",
        "r",
        (thandle_t)&mf2,
        zero_read,
        zero_write,
        zero_seek,
        zero_close,
        zero_size,
        zero_map,
        zero_unmap
    );

    if (tif2) {
        TIFFReadWriteProc rp2 = TIFFGetReadProc(tif2);
        TIFFCloseProc cp2 = TIFFGetCloseProc(tif2);
        TIFFSizeProc sp2 = TIFFGetSizeProc(tif2);
        thandle_t prev = TIFFSetClientdata(tif2, (thandle_t)&mf);
        (void)TIFFClientdata(tif2);
        (void)TIFFSetClientdata(tif2, prev);

        if (rp2) {
            uint8_t tmp2[8];
            rp2(TIFFClientdata(tif2), tmp2, (tmsize_t)sizeof(tmp2));
        }
        if (sp2) {
            (void)sp2(TIFFClientdata(tif2));
        }
        (void)cp2;

        TIFFClose(tif2);
    }

    free(copy);
    return 0;
}