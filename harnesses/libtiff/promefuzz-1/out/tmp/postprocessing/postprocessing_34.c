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
    if (!h || !buf || size < 0 || h->closed) return 0;
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
    if (!h || size < 0 || h->closed) return -1;
    return size;
}

static uint64_t fuzz_seek(thandle_t handle, uint64_t off, int whence) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (!h || h->closed) return (uint64_t)-1;

    size_t newpos = 0;
    switch (whence) {
        case SEEK_SET:
            newpos = (off > h->size) ? h->size : (size_t)off;
            break;
        case SEEK_CUR: {
            uint64_t cur = (uint64_t)h->pos;
            uint64_t sum = cur + off;
            newpos = (sum > h->size) ? h->size : (size_t)sum;
            break;
        }
        case SEEK_END: {
            uint64_t end = (uint64_t)h->size;
            uint64_t sum = end + off;
            newpos = (sum > h->size) ? h->size : (size_t)sum;
            break;
        }
        default:
            return (uint64_t)-1;
    }
    h->pos = newpos;
    return (uint64_t)h->pos;
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
    if (!h || !base || !size || h->closed) return 0;
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
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    FuzzHandle h1;
    h1.size = Size;
    h1.pos = 0;
    h1.closed = 0;
    h1.buf = NULL;

    if (Size > 0) {
        h1.buf = (uint8_t *)malloc(Size);
        if (!h1.buf) return 0;
        memcpy(h1.buf, Data, Size);
    }

    const char *mode = (Size > 0 && (Data[0] & 1)) ? "r" : "rm";
    TIFF *tif = TIFFClientOpen("fuzz_mem", mode, (thandle_t)&h1,
                               fuzz_read, fuzz_write, fuzz_seek, fuzz_close,
                               fuzz_size, fuzz_map, fuzz_unmap);

    if (tif) {
        (void)TIFFGetMapFileProc(tif);
        (void)TIFFGetCloseProc(tif);
        (void)TIFFGetSizeProc(tif);

        thandle_t old_cd = TIFFSetClientdata(tif, (thandle_t)&h1);
        (void)TIFFSetClientdata(tif, old_cd);

        (void)TIFFGetClientInfo(tif, "nonexistent");
        if (Size > 1) {
            size_t nlen = Data[1] % 32;
            char namebuf[33];
            size_t i;
            for (i = 0; i < nlen && 2 + i < Size; ++i) {
                unsigned char c = Data[2 + i];
                namebuf[i] = (char)('A' + (c % 26));
            }
            namebuf[i] = '\0';
            (void)TIFFGetClientInfo(tif, namebuf);
        }

        TIFFMapFileProc mproc = TIFFGetMapFileProc(tif);
        TIFFCloseProc cproc = TIFFGetCloseProc(tif);
        TIFFSizeProc sproc = TIFFGetSizeProc(tif);

        if (mproc) {
            void *base = NULL;
            toff_t msize = 0;
            if (mproc((thandle_t)&h1, &base, &msize)) {
                fuzz_unmap((thandle_t)&h1, base, msize);
            }
        }
        if (sproc) {
            (void)sproc((thandle_t)&h1);
        }
        if (cproc) {
            (void)cproc((thandle_t)&h1);
            h1.closed = 0;
        }

        TIFFClose(tif);
    }

    free(h1.buf);

    return 0;
}