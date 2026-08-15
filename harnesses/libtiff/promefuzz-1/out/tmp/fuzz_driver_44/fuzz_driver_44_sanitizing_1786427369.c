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
    toff_t size;
    toff_t pos;
    int closed;
} FuzzFile;

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    FuzzFile *f = (FuzzFile *)handle;
    if (f == NULL || f->closed || buf == NULL || size < 0) return 0;
    if (f->pos > f->size) return 0;
    toff_t remain = f->size - f->pos;
    tmsize_t n = size;
    if ((toff_t)n > remain) n = (tmsize_t)remain;
    if (n > 0) {
        memcpy(buf, f->buf + f->pos, (size_t)n);
        f->pos += (toff_t)n;
    }
    return n;
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    FuzzFile *f = (FuzzFile *)handle;
    if (f == NULL || f->closed || size < 0) return 0;
    if (size == 0) return 0;

    toff_t end = f->pos + (toff_t)size;
    if (end < f->pos) return 0;

    if (end > f->size) {
        uint8_t *newbuf = (uint8_t *)realloc(f->buf, (size_t)end);
        if (newbuf == NULL) {
            if (f->pos >= f->size) return 0;
            size = (tmsize_t)(f->size - f->pos);
            end = f->pos + (toff_t)size;
        } else {
            if (end > f->size) {
                memset(newbuf + f->size, 0, (size_t)(end - f->size));
            }
            f->buf = newbuf;
            f->size = end;
        }
    }

    if (size > 0 && buf != NULL && f->pos + (toff_t)size <= f->size) {
        memcpy(f->buf + f->pos, buf, (size_t)size);
        f->pos += (toff_t)size;
        return size;
    }
    return 0;
}

static toff_t fuzz_seek(thandle_t handle, toff_t off, int whence) {
    FuzzFile *f = (FuzzFile *)handle;
    toff_t newpos = 0;
    if (f == NULL || f->closed) return (toff_t)-1;

    switch (whence) {
        case SEEK_SET:
            newpos = off;
            break;
        case SEEK_CUR:
            newpos = f->pos + off;
            break;
        case SEEK_END:
            newpos = f->size + off;
            break;
        default:
            return (toff_t)-1;
    }

    if (newpos > f->size) {
        uint8_t *newbuf = (uint8_t *)realloc(f->buf, (size_t)newpos);
        if (newbuf == NULL) return (toff_t)-1;
        memset(newbuf + f->size, 0, (size_t)(newpos - f->size));
        f->buf = newbuf;
        f->size = newpos;
    }

    f->pos = newpos;
    return f->pos;
}

static int fuzz_close(thandle_t handle) {
    FuzzFile *f = (FuzzFile *)handle;
    if (f == NULL) return 0;
    f->closed = 1;
    return 0;
}

static toff_t fuzz_size(thandle_t handle) {
    FuzzFile *f = (FuzzFile *)handle;
    if (f == NULL) return 0;
    return f->size;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    FuzzFile *f = (FuzzFile *)handle;
    if (f == NULL || base == NULL || size == NULL) return 0;
    *base = f->buf;
    *size = f->size;
    return 1;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Data != NULL && Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    FuzzFile ff;
    memset(&ff, 0, sizeof(ff));

    if (Size > 0) {
        ff.buf = (uint8_t *)malloc(Size);
        if (ff.buf == NULL) return 0;
        memcpy(ff.buf, Data, Size);
        ff.size = (toff_t)Size;
    }

    const char *modes[] = {
        "r", "rM", "rm", "w", "w8", "a", "r+"
    };
    size_t i;
    for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        ff.pos = 0;
        ff.closed = 0;

        TIFF *tif = TIFFClientOpen(
            "fuzz_mem",
            modes[i],
            (thandle_t)&ff,
            fuzz_read,
            fuzz_write,
            fuzz_seek,
            fuzz_close,
            fuzz_size,
            fuzz_map,
            fuzz_unmap);

        if (tif != NULL) {
            (void)TIFFClientdata(tif);

            TIFFReadWriteProc rproc = TIFFGetReadProc(tif);
            TIFFReadWriteProc wproc = TIFFGetWriteProc(tif);
            TIFFSeekProc sproc = TIFFGetSeekProc(tif);

            if (rproc != NULL) {
                uint8_t tmp[32];
                memset(tmp, 0, sizeof(tmp));
                ff.pos = 0;
                (void)rproc((thandle_t)&ff, tmp, (tmsize_t)(Size % sizeof(tmp)));
                (void)rproc((thandle_t)&ff, tmp, (tmsize_t)sizeof(tmp));
            }

            if (sproc != NULL) {
                (void)sproc((thandle_t)&ff, 0, SEEK_SET);
                (void)sproc((thandle_t)&ff, (toff_t)(Size / 2), SEEK_SET);
                (void)sproc((thandle_t)&ff, 0, SEEK_END);
            }

            if (wproc != NULL && Size > 0) {
                ff.pos = 0;
                (void)wproc((thandle_t)&ff, (void *)Data, (tmsize_t)(Size > 64 ? 64 : Size));
            }

            TIFFCleanup(tif);
        }
    }

    free(ff.buf);
    return 0;
}