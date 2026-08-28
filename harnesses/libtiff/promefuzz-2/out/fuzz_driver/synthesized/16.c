// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetReadProc at tif_open.c:676:1 in tiffio.h
// TIFFGetWriteProc at tif_open.c:685:1 in tiffio.h
// TIFFGetSeekProc at tif_open.c:694:1 in tiffio.h
// TIFFGetMapFileProc at tif_open.c:721:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFGetUnmapFileProc at tif_open.c:730:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
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
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#ifndef OPEN_FLAGS_RDONLY
#define OPEN_FLAGS_RDONLY (_O_BINARY | _O_RDONLY)
#endif
#ifndef OPEN_FLAGS_RDWR
#define OPEN_FLAGS_RDWR (_O_BINARY | _O_RDWR)
#endif
#ifndef OPEN_FLAGS_CREAT_TRUNC
#define OPEN_FLAGS_CREAT_TRUNC (_O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR)
#endif
#define CLOSE_FD _close
#define OPEN_FD _open
#define WRITE_FD _write
#define LSEEK_FD _lseek
#else
#include <unistd.h>
#ifndef OPEN_FLAGS_RDONLY
#define OPEN_FLAGS_RDONLY O_RDONLY
#endif
#ifndef OPEN_FLAGS_RDWR
#define OPEN_FLAGS_RDWR O_RDWR
#endif
#ifndef OPEN_FLAGS_CREAT_TRUNC
#define OPEN_FLAGS_CREAT_TRUNC (O_CREAT | O_TRUNC | O_RDWR)
#endif
#define CLOSE_FD close
#define OPEN_FD open
#define WRITE_FD write
#define LSEEK_FD lseek
#endif

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t capacity;
    uint64_t pos;
    int writable;
    int allow_map;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || buf == NULL || size < 0) return -1;
    if (mf->pos > mf->size) return 0;
    size_t avail = mf->size - (size_t)mf->pos;
    size_t n = (size_t)size;
    if (n > avail) n = avail;
    if (n > 0) memcpy(buf, mf->buf + mf->pos, n);
    mf->pos += n;
    return (tmsize_t)n;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || size < 0) return -1;
    if (!mf->writable) return -1;

    size_t n = (size_t)size;
    uint64_t end64 = mf->pos + n;
    if (end64 < mf->pos) return -1;
    if (end64 > mf->capacity) {
        size_t newcap = mf->capacity ? mf->capacity : 1;
        while ((uint64_t)newcap < end64) {
            if (newcap > ((size_t)-1) / 2) {
                newcap = (size_t)end64;
                break;
            }
            newcap *= 2;
        }
        uint8_t *newbuf = (uint8_t *)realloc(mf->buf, newcap);
        if (newbuf == NULL) return -1;
        if (newcap > mf->capacity) {
            memset(newbuf + mf->capacity, 0, newcap - mf->capacity);
        }
        mf->buf = newbuf;
        mf->capacity = newcap;
    }

    if (n > 0 && buf != NULL) memcpy(mf->buf + mf->pos, buf, n);
    mf->pos = end64;
    if (mf->pos > mf->size) mf->size = (size_t)mf->pos;
    return (tmsize_t)n;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL) return (uint64_t)-1;

    uint64_t base = 0;
    if (whence == SEEK_SET) base = 0;
    else if (whence == SEEK_CUR) base = mf->pos;
    else if (whence == SEEK_END) base = mf->size;
    else return (uint64_t)-1;

    if (off > UINT64_MAX - base) return (uint64_t)-1;
    mf->pos = base + off;
    return mf->pos;
}

static int mem_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static uint64_t mem_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL) return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || base == NULL || size == NULL) return 0;
    if (!mf->allow_map) return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void exercise_tiff(TIFF *tif) {
    if (tif == NULL) return;

    TIFFReadWriteProc rp = TIFFGetReadProc(tif);
    TIFFReadWriteProc wp = TIFFGetWriteProc(tif);
    TIFFSeekProc sp = TIFFGetSeekProc(tif);
    TIFFMapFileProc mp = TIFFGetMapFileProc(tif);

    if (rp != NULL) {
        unsigned char tmp[32];
        (void)rp(TIFFClientdata(tif), tmp, (tmsize_t)(sizeof(tmp) / 2));
        (void)rp(TIFFClientdata(tif), tmp, (tmsize_t)sizeof(tmp));
    }
    if (sp != NULL) {
        (void)sp(TIFFClientdata(tif), 0, SEEK_SET);
        (void)sp(TIFFClientdata(tif), 1, SEEK_CUR);
        (void)sp(TIFFClientdata(tif), 0, SEEK_END);
    }
    if (wp != NULL) {
        unsigned char out[16];
        memset(out, 0xA5, sizeof(out));
        (void)wp(TIFFClientdata(tif), out, (tmsize_t)(sizeof(out) / 2));
    }
    if (mp != NULL) {
        void *base = NULL;
        toff_t sz = 0;
        if (mp(TIFFClientdata(tif), &base, &sz)) {
            TIFFUnmapFileProc up = TIFFGetUnmapFileProc(tif);
            if (up != NULL) up(TIFFClientdata(tif), base, sz);
        }
    }
}

int LLVMFuzzerTestOneInput_16(const uint8_t *Data, size_t Size) {
    MemFile mf1;
    memset(&mf1, 0, sizeof(mf1));
    mf1.writable = 1;
    mf1.allow_map = (Size > 0) ? (Data[0] & 1) : 0;

    if (Size > 0) {
        mf1.buf = (uint8_t *)malloc(Size);
        if (mf1.buf == NULL) return 0;
        memcpy(mf1.buf, Data, Size);
        mf1.size = Size;
        mf1.capacity = Size;
    }

    TIFF *tif1 = TIFFClientOpen(
        "mem-ro",
        "r",
        (thandle_t)&mf1,
        mem_read,
        mem_write,
        mem_seek,
        mem_close,
        mem_size,
        mem_map,
        mem_unmap
    );
    exercise_tiff(tif1);
    if (tif1 != NULL) TIFFClose(tif1);
    free(mf1.buf);

    MemFile mf2;
    memset(&mf2, 0, sizeof(mf2));
    mf2.writable = 1;
    mf2.allow_map = (Size > 1) ? (Data[1] & 1) : 1;

    if (Size > 0) {
        mf2.buf = (uint8_t *)malloc(Size);
        if (mf2.buf == NULL) return 0;
        memcpy(mf2.buf, Data, Size);
        mf2.size = Size;
        mf2.capacity = Size;
    }

    TIFF *tif2 = TIFFClientOpen(
        "mem-rw",
        "r+",
        (thandle_t)&mf2,
        mem_read,
        mem_write,
        mem_seek,
        mem_close,
        mem_size,
        mem_map,
        mem_unmap
    );
    exercise_tiff(tif2);
    if (tif2 != NULL) TIFFClose(tif2);
    free(mf2.buf);

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) (void)fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    int fd1 = OPEN_FD("./dummy_file", OPEN_FLAGS_RDONLY, 0600);
    if (fd1 >= 0) {
        TIFF *tif3 = TIFFFdOpen(fd1, "./dummy_file", "r");
        exercise_tiff(tif3);
        if (tif3 != NULL) {
            TIFFClose(tif3);
        } else {
            CLOSE_FD(fd1);
        }
    }

    int fd2 = OPEN_FD("./dummy_file", OPEN_FLAGS_RDWR, 0600);
    if (fd2 >= 0) {
        TIFF *tif4 = TIFFFdOpen(fd2, "./dummy_file", "r+");
        exercise_tiff(tif4);
        if (tif4 != NULL) {
            TIFFClose(tif4);
        } else {
            CLOSE_FD(fd2);
        }
    }

    int fd3 = OPEN_FD("./dummy_file", OPEN_FLAGS_CREAT_TRUNC, 0600);
    if (fd3 >= 0) {
        if (Size > 0) {
            (void)WRITE_FD(fd3, Data, Size);
            (void)LSEEK_FD(fd3, 0, SEEK_SET);
        }
        TIFF *tif5 = TIFFFdOpen(fd3, "./dummy_file", "w");
        exercise_tiff(tif5);
        if (tif5 != NULL) {
            TIFFClose(tif5);
        } else {
            CLOSE_FD(fd3);
        }
    }

    return 0;
}