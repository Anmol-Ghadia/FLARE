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
    toff_t cap;
    toff_t pos;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || buf == NULL || size < 0) return -1;
    if (mf->pos > mf->size) return 0;
    toff_t avail = mf->size - mf->pos;
    tmsize_t n = size;
    if ((toff_t)n > avail) n = (tmsize_t)avail;
    if (n > 0) {
        memcpy(buf, mf->buf + mf->pos, (size_t)n);
        mf->pos += (toff_t)n;
    }
    return n;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || size < 0) return -1;
    if (size == 0) return 0;

    if (mf->pos > mf->cap) return -1;
    if ((uint64_t)mf->pos + (uint64_t)size > (uint64_t)mf->cap) {
        toff_t newcap = mf->cap ? mf->cap : 64;
        while ((uint64_t)newcap < (uint64_t)mf->pos + (uint64_t)size) {
            if (newcap > (toff_t)(1U << 30)) return -1;
            newcap *= 2;
        }
        uint8_t *newbuf = (uint8_t *)realloc(mf->buf, (size_t)newcap);
        if (newbuf == NULL) return -1;
        mf->buf = newbuf;
        mf->cap = newcap;
    }

    if (buf != NULL) {
        memcpy(mf->buf + mf->pos, buf, (size_t)size);
    } else {
        memset(mf->buf + mf->pos, 0, (size_t)size);
    }
    mf->pos += (toff_t)size;
    if (mf->pos > mf->size) mf->size = mf->pos;
    return size;
}

static toff_t mem_seek(thandle_t handle, toff_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    toff_t newpos = 0;
    if (mf == NULL) return (toff_t)-1;

    switch (whence) {
        case SEEK_SET:
            newpos = off;
            break;
        case SEEK_CUR:
            newpos = mf->pos + off;
            break;
        case SEEK_END:
            newpos = mf->size + off;
            break;
        default:
            return (toff_t)-1;
    }

    if ((int64_t)newpos < 0) return (toff_t)-1;
    mf->pos = newpos;
    return mf->pos;
}

static int mem_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static toff_t mem_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL) return 0;
    return mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || base == NULL || size == NULL) return 0;
    *base = mf->buf;
    *size = mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

static void init_memfile(MemFile *mf, const uint8_t *data, size_t size) {
    if (mf == NULL) return;
    mf->buf = NULL;
    mf->size = 0;
    mf->cap = 0;
    mf->pos = 0;

    if (size == 0) return;

    mf->buf = (uint8_t *)malloc(size);
    if (mf->buf == NULL) return;
    memcpy(mf->buf, data, size);
    mf->size = (toff_t)size;
    mf->cap = (toff_t)size;
}

static void free_memfile(MemFile *mf) {
    if (mf == NULL) return;
    free(mf->buf);
    mf->buf = NULL;
    mf->size = 0;
    mf->cap = 0;
    mf->pos = 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    MemFile mf;
    TIFF *tif = NULL;
    const char *mode;
    char name1[32];
    char name2[32];
    char key1[32];
    char key2[32];
    uint8_t client_blob1[16];
    uint8_t client_blob2[16];
    thandle_t old_handle;
    const char *old_name;
    void *got1;
    void *got2;
    void *got3;

    init_memfile(&mf, Data, Size);

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp != NULL) {
            if (Size > 0 && Data != NULL) {
                fwrite(Data, 1, Size, fp);
            }
            fclose(fp);
        }
    }

    mode = (Size > 0 && (Data[0] & 1)) ? "r" : "w";
    tif = TIFFClientOpen(
        "memtiff",
        mode,
        (thandle_t)&mf,
        mem_read,
        mem_write,
        mem_seek,
        mem_close,
        mem_size,
        mem_map,
        mem_unmap
    );

    if (tif == NULL) {
        free_memfile(&mf);
        return 0;
    }

    memset(name1, 0, sizeof(name1));
    memset(name2, 0, sizeof(name2));
    memset(key1, 0, sizeof(key1));
    memset(key2, 0, sizeof(key2));
    memset(client_blob1, 0, sizeof(client_blob1));
    memset(client_blob2, 0, sizeof(client_blob2));

    if (Size > 1) {
        size_t n = Size - 1;
        if (n > sizeof(name1) - 1) n = sizeof(name1) - 1;
        memcpy(name1, Data + 1, n);
    } else {
        memcpy(name1, "n1", 2);
    }

    if (Size > 2) {
        size_t off = Size / 3;
        size_t n = Size - off;
        if (n > sizeof(name2) - 1) n = sizeof(name2) - 1;
        memcpy(name2, Data + off, n);
    } else {
        memcpy(name2, "n2", 2);
    }

    if (Size > 3) {
        size_t off = Size / 4;
        size_t n = Size - off;
        if (n > sizeof(key1) - 1) n = sizeof(key1) - 1;
        memcpy(key1, Data + off, n);
    } else {
        memcpy(key1, "key1", 4);
    }

    if (Size > 4) {
        size_t off = Size / 2;
        size_t n = Size - off;
        if (n > sizeof(key2) - 1) n = sizeof(key2) - 1;
        memcpy(key2, Data + off, n);
    } else {
        memcpy(key2, "key2", 4);
    }

    if (Size > 0) {
        size_t n = Size < sizeof(client_blob1) ? Size : sizeof(client_blob1);
        memcpy(client_blob1, Data, n);
    }
    if (Size > 5) {
        size_t off = 5;
        size_t n = Size - off;
        if (n > sizeof(client_blob2)) n = sizeof(client_blob2);
        memcpy(client_blob2, Data + off, n);
    }

    old_handle = TIFFSetClientdata(tif, (thandle_t)&mf);
    (void)old_handle;
    old_handle = TIFFSetClientdata(tif, (thandle_t)client_blob1);
    old_handle = TIFFSetClientdata(tif, old_handle);

    old_name = TIFFSetFileName(tif, name1);
    (void)old_name;
    old_name = TIFFSetFileName(tif, name2);
    old_name = TIFFSetFileName(tif, NULL);
    old_name = TIFFSetFileName(tif, name1);

    TIFFSetClientInfo(tif, client_blob1, key1);
    TIFFSetClientInfo(tif, client_blob2, key2);
    TIFFSetClientInfo(tif, &mf, key1);

    got1 = TIFFGetClientInfo(tif, key1);
    got2 = TIFFGetClientInfo(tif, key2);
    got3 = TIFFGetClientInfo(tif, name1);

    if (got1 == got2 && got1 == got3) {
        TIFFSetClientInfo(tif, got1, "same");
        (void)TIFFGetClientInfo(tif, "same");
    } else {
        if (got1 != NULL) TIFFSetClientInfo(tif, got1, "a");
        if (got2 != NULL) TIFFSetClientInfo(tif, got2, "b");
        if (got3 != NULL) TIFFSetClientInfo(tif, got3, "c");
        (void)TIFFGetClientInfo(tif, "a");
        (void)TIFFGetClientInfo(tif, "b");
        (void)TIFFGetClientInfo(tif, "c");
    }

    TIFFCleanup(tif);
    free_memfile(&mf);
    return 0;
}