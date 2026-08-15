// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFAccessTagMethods at tif_extension.c:58:17 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
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
    tmsize_t size;
    tmsize_t pos;
} MemFile;

static uint16_t rd16(const uint8_t **p, size_t *n) {
    uint16_t v = 0;
    if (*n >= 2) {
        v = (uint16_t)((*p)[0] | ((uint16_t)(*p)[1] << 8));
        *p += 2;
        *n -= 2;
    } else if (*n == 1) {
        v = (*p)[0];
        *p += 1;
        *n = 0;
    }
    return v;
}

static uint32_t rd32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t i, m = *n < 4 ? *n : 4;
    for (i = 0; i < m; i++) {
        v |= ((uint32_t)(*p)[i]) << (8 * i);
    }
    *p += m;
    *n -= m;
    return v;
}

static char *make_string(const uint8_t **p, size_t *n, size_t maxlen) {
    size_t len;
    char *s;

    if (*n == 0) {
        s = (char *)malloc(1);
        if (s) s[0] = '\0';
        return s;
    }

    len = (**p) % (maxlen + 1);
    (*p)++;
    (*n)--;

    if (len > *n)
        len = *n;

    s = (char *)malloc(len + 1);
    if (!s)
        return NULL;

    if (len)
        memcpy(s, *p, len);
    s[len] = '\0';
    *p += len;
    *n -= len;
    return s;
}

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    tmsize_t avail;
    if (!mf || !buf || size < 0)
        return 0;
    if (mf->pos < 0 || mf->size < 0 || mf->pos > mf->size)
        return 0;
    avail = mf->size - mf->pos;
    if (size > avail)
        size = avail;
    if (size > 0) {
        memcpy(buf, mf->buf + mf->pos, (size_t)size);
        mf->pos += size;
    }
    return size;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size) {
    MemFile *mf = (MemFile *)handle;
    uint8_t *newbuf;
    tmsize_t newsize;
    if (!mf || size < 0)
        return 0;
    if (size == 0)
        return 0;
    if (mf->pos < 0)
        return 0;
    newsize = mf->pos + size;
    if (newsize < mf->pos)
        return 0;
    if (newsize > mf->size) {
        newbuf = (uint8_t *)realloc(mf->buf, (size_t)newsize);
        if (!newbuf)
            return 0;
        if (newsize > mf->size)
            memset(newbuf + mf->size, 0, (size_t)(newsize - mf->size));
        mf->buf = newbuf;
        mf->size = newsize;
    }
    if (buf)
        memcpy(mf->buf + mf->pos, buf, (size_t)size);
    mf->pos += size;
    return size;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence) {
    MemFile *mf = (MemFile *)handle;
    uint64_t base = 0, newpos;
    if (!mf)
        return (uint64_t)-1;

    if (whence == SEEK_SET)
        base = 0;
    else if (whence == SEEK_CUR)
        base = (mf->pos >= 0) ? (uint64_t)mf->pos : 0;
    else if (whence == SEEK_END)
        base = (mf->size >= 0) ? (uint64_t)mf->size : 0;
    else
        return (uint64_t)-1;

    newpos = base + off;
    if (newpos > (uint64_t)((tmsize_t)-1))
        return (uint64_t)-1;
    mf->pos = (tmsize_t)newpos;
    return newpos;
}

static int mem_close(thandle_t handle) {
    (void)handle;
    return 0;
}

static uint64_t mem_size(thandle_t handle) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || mf->size < 0)
        return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, uint64_t *size) {
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size)
        return 0;
    *base = mf->buf;
    *size = (mf->size >= 0) ? (uint64_t)mf->size : 0;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, uint64_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    MemFile mf;
    TIFF *tif = NULL;
    char *open_name = NULL;
    char *mode = NULL;
    char *name1 = NULL, *name2 = NULL, *name3 = NULL;
    char *fname1 = NULL, *fname2 = NULL;
    void *old_cd, *tmp1, *tmp2;
    const char *old_fname;
    TIFFTagMethods *methods;
    uint8_t *blob1 = NULL, *blob2 = NULL, *blob3 = NULL;
    size_t blob1_len = 0, blob2_len = 0, blob3_len = 0;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    memset(&mf, 0, sizeof(mf));
    if (Size > 0) {
        mf.buf = (uint8_t *)malloc(Size);
        if (!mf.buf)
            return 0;
        memcpy(mf.buf, Data, Size);
        mf.size = (tmsize_t)Size;
    } else {
        mf.buf = NULL;
        mf.size = 0;
    }
    mf.pos = 0;

    open_name = make_string(&p, &n, 32);
    mode = make_string(&p, &n, 4);
    if (!open_name || !mode)
        goto cleanup;

    if (mode[0] == '\0') {
        free(mode);
        mode = (char *)malloc(3);
        if (!mode)
            goto cleanup;
        mode[0] = 'r';
        mode[1] = 'm';
        mode[2] = '\0';
    }

    tif = TIFFClientOpen(open_name, mode, (thandle_t)&mf,
                         mem_read, mem_write, mem_seek, mem_close,
                         mem_size, mem_map, mem_unmap);
    if (!tif) {
        free(mode);
        mode = (char *)malloc(3);
        if (!mode)
            goto cleanup;
        mode[0] = 'r';
        mode[1] = 'c';
        mode[2] = '\0';
        mf.pos = 0;
        tif = TIFFClientOpen(open_name, mode, (thandle_t)&mf,
                             mem_read, mem_write, mem_seek, mem_close,
                             mem_size, mem_map, mem_unmap);
    }
    if (!tif)
        goto cleanup;

    name1 = make_string(&p, &n, 32);
    name2 = make_string(&p, &n, 32);
    name3 = make_string(&p, &n, 32);
    fname1 = make_string(&p, &n, 64);
    fname2 = make_string(&p, &n, 64);
    if (!name1 || !name2 || !name3 || !fname1 || !fname2)
        goto cleanup;

    if (n > 0) {
        blob1_len = (*p) % 32;
        p++;
        n--;
        if (blob1_len > n)
            blob1_len = n;
        blob1 = (uint8_t *)malloc(blob1_len ? blob1_len : 1);
        if (!blob1)
            goto cleanup;
        if (blob1_len)
            memcpy(blob1, p, blob1_len);
        p += blob1_len;
        n -= blob1_len;
    }

    if (n > 0) {
        blob2_len = (*p) % 32;
        p++;
        n--;
        if (blob2_len > n)
            blob2_len = n;
        blob2 = (uint8_t *)malloc(blob2_len ? blob2_len : 1);
        if (!blob2)
            goto cleanup;
        if (blob2_len)
            memcpy(blob2, p, blob2_len);
        p += blob2_len;
        n -= blob2_len;
    }

    if (n > 0) {
        blob3_len = (*p) % 32;
        p++;
        n--;
        if (blob3_len > n)
            blob3_len = n;
        blob3 = (uint8_t *)malloc(blob3_len ? blob3_len : 1);
        if (!blob3)
            goto cleanup;
        if (blob3_len)
            memcpy(blob3, p, blob3_len);
        p += blob3_len;
        n -= blob3_len;
    }

    old_cd = TIFFSetClientdata(tif, (thandle_t)blob1);
    (void)TIFFSetClientdata(tif, old_cd);
    tmp1 = (void *)(uintptr_t)rd32(&p, &n);
    tmp2 = (void *)(uintptr_t)rd32(&p, &n);
    (void)TIFFSetClientdata(tif, (thandle_t)tmp1);
    (void)TIFFSetClientdata(tif, (thandle_t)tmp2);
    (void)TIFFSetClientdata(tif, (thandle_t)&mf);

    old_fname = TIFFSetFileName(tif, fname1);
    (void)old_fname;
    (void)TIFFSetFileName(tif, fname2);
    (void)TIFFSetFileName(tif, NULL);
    (void)TIFFSetFileName(tif, fname1);

    TIFFSetClientInfo(tif, blob1, name1 ? name1 : "");
    TIFFSetClientInfo(tif, blob2, name2 ? name2 : "");
    TIFFSetClientInfo(tif, blob3, name3 ? name3 : "");
    TIFFSetClientInfo(tif, blob2, name1 ? name1 : "");

    (void)TIFFGetClientInfo(tif, name1 ? name1 : "");
    (void)TIFFGetClientInfo(tif, name2 ? name2 : "");
    (void)TIFFGetClientInfo(tif, name3 ? name3 : "");
    (void)TIFFGetClientInfo(tif, "");
    (void)TIFFGetClientInfo(tif, "nonexistent");

    methods = TIFFAccessTagMethods(tif);
    if (methods && n > 0) {
        (void)methods;
    }

cleanup:
    if (tif)
        TIFFClose(tif);
    free(open_name);
    free(mode);
    free(name1);
    free(name2);
    free(name3);
    free(fname1);
    free(fname2);
    free(blob1);
    free(blob2);
    free(blob3);
    free(mf.buf);
    return 0;
}