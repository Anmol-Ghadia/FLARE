// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFGetReadProc at tif_open.c:676:1 in tiffio.h
// TIFFGetMapFileProc at tif_open.c:721:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFCleanup at tif_close.c:45:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
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

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <fcntl.h>
#define OPEN _open
#define CLOSE _close
#define READFLAG _O_RDONLY
#define BINARYFLAG _O_BINARY
#else
#include <unistd.h>
#include <fcntl.h>
#define OPEN open
#define CLOSE close
#define READFLAG O_RDONLY
#define BINARYFLAG 0
#endif

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t pos;
    int closed;
    int map_mode;
} MemFile;

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0 || mf->closed) return 0;
    if (mf->pos > mf->size) return 0;
    size_t remain = mf->size - mf->pos;
    size_t want = (size_t)size;
    if (want > remain) want = remain;
    if (want > 0) {
        memcpy(buf, mf->buf + mf->pos, want);
        mf->pos += want;
    }
    return (tmsize_t)want;
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    (void)handle;
    (void)buf;
    if (size < 0) return -1;
    return size;
}

static uint64_t fuzz_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    size_t newpos = 0;
    if (!mf || mf->closed) return (uint64_t)(-1);

    switch (whence) {
        case SEEK_SET:
            newpos = (size_t)off;
            break;
        case SEEK_CUR:
            if (off > (uint64_t)(SIZE_MAX - mf->pos)) return (uint64_t)(-1);
            newpos = mf->pos + (size_t)off;
            break;
        case SEEK_END:
            if (off > (uint64_t)(SIZE_MAX - mf->size)) return (uint64_t)(-1);
            newpos = mf->size + (size_t)off;
            break;
        default:
            return (uint64_t)(-1);
    }

    mf->pos = newpos;
    return (uint64_t)mf->pos;
}

static int fuzz_close(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    mf->closed = 1;
    return 0;
}

static uint64_t fuzz_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf) return 0;
    return (uint64_t)mf->size;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size || !mf->map_mode) return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void exercise_tiff(TIFF *tif) {
    if (!tif) return;

    (void)TIFFGetReadProc(tif);
    (void)TIFFGetMapFileProc(tif);
    (void)TIFFClientdata(tif);

    TIFFCleanup(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    MemFile mf1;
    MemFile mf2;
    TIFF *tif = NULL;
    FILE *fp = NULL;
    int fd = -1;

    mf1.buf = (uint8_t *)malloc(Size ? Size : 1);
    if (!mf1.buf) return 0;
    if (Size) memcpy(mf1.buf, Data, Size);
    mf1.size = Size;
    mf1.pos = 0;
    mf1.closed = 0;
    mf1.map_mode = (Size > 0) ? (Data[0] & 1) : 0;

    tif = TIFFClientOpen("mem-ro", "r",
                         (thandle_t)&mf1,
                         fuzz_read, fuzz_write,
                         fuzz_seek, fuzz_close,
                         fuzz_size,
                         fuzz_map, fuzz_unmap);
    exercise_tiff(tif);

    mf2.buf = (uint8_t *)malloc(Size ? Size : 1);
    if (mf2.buf) {
        if (Size) memcpy(mf2.buf, Data, Size);
        mf2.size = Size;
        mf2.pos = (Size > 1) ? (size_t)(Data[1] % (Size + 1)) : 0;
        mf2.closed = 0;
        mf2.map_mode = (Size > 2) ? (Data[2] & 1) : 1;

        tif = TIFFClientOpen("mem-rm", "rm",
                             (thandle_t)&mf2,
                             fuzz_read, fuzz_write,
                             fuzz_seek, fuzz_close,
                             fuzz_size,
                             fuzz_map, fuzz_unmap);
        exercise_tiff(tif);
        free(mf2.buf);
    }

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
        fp = NULL;
    }

    fd = OPEN("./dummy_file", READFLAG | BINARYFLAG);
    if (fd >= 0) {
        tif = TIFFFdOpen(fd, "./dummy_file", "r");
        if (tif) {
            exercise_tiff(tif);
        } else {
            CLOSE(fd);
        }
    }

    free(mf1.buf);
    return 0;
}