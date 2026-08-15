#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tiffio.h>

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t cap;
    uint64_t off;
    int closed;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    size_t avail, n;

    if (!mf || !buf || size <= 0) return 0;
    if (mf->off > mf->size) return 0;

    avail = mf->size - (size_t)mf->off;
    n = (size_t)size;
    if (n > avail) n = avail;
    if (n > 0) memcpy(buf, mf->buf + mf->off, n);
    mf->off += n;
    return (tmsize_t)n;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    size_t n;

    if (!mf || size <= 0) return 0;
    n = (size_t)size;

    if (mf->off > (uint64_t)SIZE_MAX) return 0;
    if (n > SIZE_MAX - (size_t)mf->off) return 0;

    if (mf->off + n > mf->cap) {
        /* Avoid realloc() so pointers previously handed out as clientdata
           remain valid for the lifetime of the TIFF handle. */
        return 0;
    }

    if (buf) memcpy(mf->buf + mf->off, buf, n);
    else memset(mf->buf + mf->off, 0, n);

    mf->off += n;
    if (mf->off > mf->size) mf->size = (size_t)mf->off;
    return (tmsize_t)n;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    uint64_t base = 0;

    if (!mf) return (uint64_t)-1;

    if (whence == SEEK_SET) base = 0;
    else if (whence == SEEK_CUR) base = mf->off;
    else if (whence == SEEK_END) base = mf->size;
    else return (uint64_t)-1;

    if (off > UINT64_MAX - base) return (uint64_t)-1;
    mf->off = base + off;
    return mf->off;
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
    if (!mf || !base || !size) return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void exercise_tiff(TIFF *tif, const uint8_t *Data, size_t Size, MemFile *mf) {
    const char *oldname;
    TIFFTagMethods *methods;
    thandle_t prev_cd;

    if (!tif) return;

    oldname = TIFFSetFileName(tif, "./dummy_file");
    (void)oldname;

    if (Size > 0) {
        size_t nlen = (Data[0] % 32) + 1;
        if (nlen > Size) nlen = Size;
        {
            char *dynname = (char *)malloc(nlen + 1);
            if (dynname) {
                memcpy(dynname, Data, nlen);
                dynname[nlen] = '\0';
                TIFFSetFileName(tif, dynname);
                TIFFSetFileName(tif, NULL);
                TIFFSetFileName(tif, dynname);
                TIFFSetFileName(tif, "./dummy_file");
                free(dynname);
            }
        }
    }

    methods = TIFFAccessTagMethods(tif);
    (void)methods;

    prev_cd = TIFFSetClientdata(tif, (thandle_t)mf);
    (void)prev_cd;
    (void)TIFFSetClientdata(tif, (thandle_t)Data);

    TIFFSetClientInfo(tif, (void *)Data, "k1");
    TIFFSetClientInfo(tif, (void *)mf, "k2");
    TIFFSetClientInfo(tif, (void *)(uintptr_t)Size, "k3");
    (void)TIFFGetClientInfo(tif, "k1");
    (void)TIFFGetClientInfo(tif, "k2");
    (void)TIFFGetClientInfo(tif, "k3");
    (void)TIFFGetClientInfo(tif, "missing");

    if (Size > 1) {
        size_t klen = (Data[1] % 24) + 1;
        if (2 + klen <= Size) {
            char *key = (char *)malloc(klen + 1);
            if (key) {
                memcpy(key, Data + 2, klen);
                key[klen] = '\0';
                TIFFSetClientInfo(tif, (void *)key, key);
                (void)TIFFGetClientInfo(tif, key);
                free(key);
            }
        }
    }

    TIFFClose(tif);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp;
    MemFile mf;
    const char *modes[] = { "r", "rm", "rC", "w", "w8", "a" };
    size_t i;
    size_t cap;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size > 0) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&mf, 0, sizeof(mf));

    cap = Size;
    if (cap < 4096) cap = 4096;

    mf.buf = (uint8_t *)malloc(cap);
    if (!mf.buf) return 0;
    mf.cap = cap;
    mf.size = Size;
    if (Size > 0) memcpy(mf.buf, Data, Size);
    if (cap > Size) memset(mf.buf + Size, 0, cap - Size);

    for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        mf.off = 0;
        mf.closed = 0;

        {
            TIFF *tif = TIFFClientOpen(
                "memtiff",
                modes[i],
                (thandle_t)&mf,
                mem_read,
                mem_write,
                mem_seek,
                mem_close,
                mem_size,
                mem_map,
                mem_unmap);
            exercise_tiff(tif, Data, Size, &mf);
        }
    }

    free(mf.buf);
    return 0;
}