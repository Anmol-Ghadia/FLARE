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

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t pos;
    int closed;
} FuzzHandle;

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h || h->closed || size < 0) return 0;
    if (h->pos > h->size) return 0;
    size_t avail = h->size - h->pos;
    size_t want = (size_t)size;
    if (want > avail) want = avail;
    if (want > 0) {
        memcpy(buf, h->buf + h->pos, want);
        h->pos += want;
    }
    return (tmsize_t)want;
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    (void)buf;
    if (!h || h->closed || size < 0) return 0;
    if ((size_t)size > h->size - h->pos) {
        size_t new_size = h->pos + (size_t)size;
        uint8_t *new_buf = (uint8_t *)realloc(h->buf, new_size ? new_size : 1);
        if (!new_buf) return 0;
        if (new_size > h->size) {
            memset(new_buf + h->size, 0, new_size - h->size);
        }
        h->buf = new_buf;
        h->size = new_size;
    }
    h->pos += (size_t)size;
    return size;
}

static uint64_t fuzz_seek(thandle_t handle, uint64_t off, int whence) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h || h->closed) return (uint64_t)-1;

    uint64_t base = 0;
    if (whence == SEEK_SET) {
        base = 0;
    } else if (whence == SEEK_CUR) {
        base = (uint64_t)h->pos;
    } else if (whence == SEEK_END) {
        base = (uint64_t)h->size;
    } else {
        return (uint64_t)-1;
    }

    if (off > UINT64_MAX - base) return (uint64_t)-1;
    {
        uint64_t newpos = base + off;
        if (newpos > (uint64_t)SIZE_MAX) return (uint64_t)-1;
        h->pos = (size_t)newpos;
        return newpos;
    }
}

static int fuzz_close(thandle_t handle) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return 0;
    h->closed = 1;
    return 0;
}

static uint64_t fuzz_size(thandle_t handle) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h) return 0;
    return (uint64_t)h->size;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h || !base || !size) return 0;
    *base = h->buf;
    *size = (toff_t)h->size;
    return 1;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    FuzzHandle h1, h2;
    TIFF *tif;
    TIFFReadWriteProc rproc, wproc;
    TIFFSeekProc sproc;
    TIFFMapFileProc mproc;
    TIFFCloseProc cproc;
    uint8_t *copy1 = NULL;
    uint8_t *copy2 = NULL;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    copy1 = (uint8_t *)malloc(Size ? Size : 1);
    if (!copy1) return 0;
    if (Size > 0) memcpy(copy1, Data, Size);

    h1.buf = copy1;
    h1.size = Size;
    h1.pos = 0;
    h1.closed = 0;

    tif = TIFFClientOpen("mem-read", "r", (thandle_t)&h1,
                         fuzz_read, fuzz_write, fuzz_seek, fuzz_close,
                         fuzz_size, fuzz_map, fuzz_unmap);
    if (tif) {
        rproc = TIFFGetReadProc(tif);
        wproc = TIFFGetWriteProc(tif);
        sproc = TIFFGetSeekProc(tif);
        mproc = TIFFGetMapFileProc(tif);
        cproc = TIFFGetCloseProc(tif);

        if (rproc) {
            uint8_t tmp[16];
            (void)rproc((thandle_t)&h1, tmp, (tmsize_t)(Size > sizeof(tmp) ? sizeof(tmp) : Size));
        }
        if (sproc) {
            (void)sproc((thandle_t)&h1, 0, SEEK_SET);
            (void)sproc((thandle_t)&h1, (uint64_t)(Size / 2), SEEK_SET);
            (void)sproc((thandle_t)&h1, 0, SEEK_END);
        }
        if (wproc) {
            uint8_t out[8] = {0};
            (void)wproc((thandle_t)&h1, out, (tmsize_t)(sizeof(out)));
        }
        if (mproc) {
            void *base = NULL;
            toff_t map_size = 0;
            if (mproc((thandle_t)&h1, &base, &map_size)) {
                fuzz_unmap((thandle_t)&h1, base, map_size);
            }
        }
        if (cproc) {
            (void)cproc((thandle_t)&h1);
            h1.closed = 0;
        }

        TIFFClose(tif);
    }

    copy2 = (uint8_t *)malloc(Size ? Size : 1);
    if (copy2) {
        if (Size > 0) memcpy(copy2, Data, Size);
        h2.buf = copy2;
        h2.size = Size;
        h2.pos = 0;
        h2.closed = 0;

        tif = TIFFClientOpen("mem-write", "w", (thandle_t)&h2,
                             fuzz_read, fuzz_write, fuzz_seek, fuzz_close,
                             fuzz_size, fuzz_map, fuzz_unmap);
        if (tif) {
            rproc = TIFFGetReadProc(tif);
            wproc = TIFFGetWriteProc(tif);
            sproc = TIFFGetSeekProc(tif);
            mproc = TIFFGetMapFileProc(tif);
            cproc = TIFFGetCloseProc(tif);

            if (wproc) {
                size_t chunk = Size > 32 ? 32 : Size;
                (void)wproc((thandle_t)&h2, (void *)Data, (tmsize_t)chunk);
            }
            if (sproc) {
                (void)sproc((thandle_t)&h2, 0, SEEK_SET);
                (void)sproc((thandle_t)&h2, (uint64_t)(Size / 3), SEEK_SET);
            }
            if (rproc) {
                uint8_t tmp[8];
                (void)rproc((thandle_t)&h2, tmp, (tmsize_t)sizeof(tmp));
            }
            if (mproc) {
                void *base = NULL;
                toff_t map_size = 0;
                if (mproc((thandle_t)&h2, &base, &map_size)) {
                    fuzz_unmap((thandle_t)&h2, base, map_size);
                }
            }
            if (cproc) {
                (void)cproc((thandle_t)&h2);
                h2.closed = 0;
            }

            TIFFClose(tif);
        }

        free(copy2);
    }

    free(copy1);
    return 0;
}