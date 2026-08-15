// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFGetMapFileProc at tif_open.c:721:1 in tiffio.h
// TIFFGetUnmapFileProc at tif_open.c:730:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
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
    toff_t size;
    toff_t capacity;
    toff_t pos;
    int closed;
} FuzzHandle;

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (h == NULL || h->closed || buf == NULL || size < 0) {
        return 0;
    }
    if (h->pos < 0 || h->pos > h->size) {
        return 0;
    }
    toff_t avail = h->size - h->pos;
    tmsize_t n = size;
    if ((toff_t)n > avail) {
        n = (tmsize_t)avail;
    }
    if (n > 0) {
        memcpy(buf, h->buf + h->pos, (size_t)n);
        h->pos += (toff_t)n;
    }
    return n;
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (h == NULL || h->closed || buf == NULL || size < 0) {
        return 0;
    }
    if (h->pos < 0) {
        return 0;
    }

    toff_t end = h->pos + (toff_t)size;
    if (end < h->pos) {
        return 0;
    }

    if (end > h->capacity) {
        toff_t newcap = h->capacity ? h->capacity : 1;
        while (newcap < end && newcap < (toff_t)(1U << 26)) {
            newcap *= 2;
        }
        if (newcap < end) {
            return 0;
        }
        uint8_t *newbuf = (uint8_t *)realloc(h->buf, (size_t)newcap);
        if (newbuf == NULL) {
            return 0;
        }
        if (newcap > h->capacity) {
            memset(newbuf + h->capacity, 0, (size_t)(newcap - h->capacity));
        }
        h->buf = newbuf;
        h->capacity = newcap;
    }

    memcpy(h->buf + h->pos, buf, (size_t)size);
    h->pos = end;
    if (h->pos > h->size) {
        h->size = h->pos;
    }
    return size;
}

static toff_t fuzz_seek(thandle_t handle, toff_t off, int whence) {
    FuzzHandle *h = (FuzzHandle *)handle;
    toff_t newpos = 0;
    if (h == NULL || h->closed) {
        return (toff_t)-1;
    }

    switch (whence) {
        case SEEK_SET:
            newpos = off;
            break;
        case SEEK_CUR:
            newpos = h->pos + off;
            break;
        case SEEK_END:
            newpos = h->size + off;
            break;
        default:
            return (toff_t)-1;
    }

    if (newpos < 0) {
        return (toff_t)-1;
    }
    h->pos = newpos;
    return h->pos;
}

static int fuzz_close(thandle_t handle) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (h != NULL) {
        h->closed = 1;
    }
    return 0;
}

static toff_t fuzz_size(thandle_t handle) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (h == NULL) {
        return 0;
    }
    return h->size;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    FuzzHandle *h = (FuzzHandle *)handle;
    if (h == NULL || base == NULL || size == NULL || h->closed) {
        return 0;
    }
    *base = h->buf;
    *size = h->size;
    return 1;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void run_mode(const uint8_t *Data, size_t Size, const char *mode, int with_map) {
    FuzzHandle h;
    TIFF *tif;
    TIFFMapFileProc mapproc = NULL;
    TIFFUnmapFileProc unmapproc = NULL;
    thandle_t old_cd;
    char namebuf[64];
    const char *query_names[] = {
        "",
        "a",
        "client",
        "dummy",
        "TIFF",
        "unknown",
        (const char *)"0123456789"
    };
    size_t i;

    memset(&h, 0, sizeof(h));
    if (Size > 0) {
        h.buf = (uint8_t *)malloc(Size);
        if (h.buf == NULL) {
            return;
        }
        memcpy(h.buf, Data, Size);
        h.size = (toff_t)Size;
        h.capacity = (toff_t)Size;
    }

    snprintf(namebuf, sizeof(namebuf), "mem-%s", mode);
    tif = TIFFClientOpen(namebuf, mode, (thandle_t)&h,
                         fuzz_read, fuzz_write, fuzz_seek, fuzz_close,
                         fuzz_size,
                         with_map ? fuzz_map : NULL,
                         with_map ? fuzz_unmap : NULL);
    if (tif == NULL) {
        free(h.buf);
        return;
    }

    mapproc = TIFFGetMapFileProc(tif);
    unmapproc = TIFFGetUnmapFileProc(tif);
    (void)mapproc;
    (void)unmapproc;

    old_cd = TIFFSetClientdata(tif, (thandle_t)&h);
    (void)old_cd;
    (void)TIFFSetClientdata(tif, (thandle_t)Data);
    (void)TIFFSetClientdata(tif, old_cd);

    for (i = 0; i < sizeof(query_names) / sizeof(query_names[0]); i++) {
        (void)TIFFGetClientInfo(tif, query_names[i]);
    }

    if (Size > 0) {
        size_t n = Size < 31 ? Size : 31;
        char dynname[32];
        memcpy(dynname, Data, n);
        dynname[n] = '\0';
        (void)TIFFGetClientInfo(tif, dynname);
    }

    TIFFCleanup(tif);
    free(h.buf);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    run_mode(Data, Size, "r", 0);
    run_mode(Data, Size, "r", 1);
    run_mode(Data, Size, "rm", 1);
    run_mode(Data, Size, "w", 0);
    run_mode(Data, Size, "w", 1);
    run_mode(Data, Size, "w8", 0);
    run_mode(Data, Size, "w8", 1);

    return 0;
}