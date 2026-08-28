// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClientdata at tif_open.c:554:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
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

typedef struct
{
    uint8_t *buf;
    size_t size;
    size_t cap;
    size_t off;
} MemFile;

static size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

static uint16_t rd16(const uint8_t **p, size_t *n)
{
    uint16_t v = 0;
    if (*n >= 1)
    {
        v |= (uint16_t)(*p)[0];
        (*p)++;
        (*n)--;
    }
    if (*n >= 1)
    {
        v |= (uint16_t)((uint16_t)(*p)[0] << 8);
        (*p)++;
        (*n)--;
    }
    return v;
}

static uint32_t rd32(const uint8_t **p, size_t *n)
{
    uint32_t v = 0;
    int i;
    for (i = 0; i < 4 && *n > 0; i++)
    {
        v |= ((uint32_t)(*p)[0]) << (8 * i);
        (*p)++;
        (*n)--;
    }
    return v;
}

static void ensure_cap(MemFile *mf, size_t need)
{
    if (need <= mf->cap)
        return;
    size_t newcap = mf->cap ? mf->cap : 64;
    while (newcap < need)
    {
        if (newcap > (size_t)-1 / 2)
        {
            newcap = need;
            break;
        }
        newcap *= 2;
    }
    uint8_t *nbuf = (uint8_t *)realloc(mf->buf, newcap);
    if (!nbuf)
        return;
    if (newcap > mf->cap)
        memset(nbuf + mf->cap, 0, newcap - mf->cap);
    mf->buf = nbuf;
    mf->cap = newcap;
}

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size <= 0)
        return 0;
    size_t avail = (mf->off < mf->size) ? (mf->size - mf->off) : 0;
    size_t n = min_size((size_t)size, avail);
    if (n > 0)
    {
        memcpy(buf, mf->buf + mf->off, n);
        mf->off += n;
    }
    return (tmsize_t)n;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || size <= 0)
        return 0;
    size_t n = (size_t)size;
    size_t need = mf->off + n;
    ensure_cap(mf, need);
    if (mf->cap < need)
    {
        if (mf->off >= mf->cap)
            return 0;
        n = mf->cap - mf->off;
        need = mf->off + n;
    }
    if (n > 0 && buf)
        memcpy(mf->buf + mf->off, buf, n);
    mf->off += n;
    if (mf->size < mf->off)
        mf->size = mf->off;
    return (tmsize_t)n;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf)
        return (uint64_t)-1;

    uint64_t base = 0;
    if (whence == SEEK_SET)
        base = 0;
    else if (whence == SEEK_CUR)
        base = (uint64_t)mf->off;
    else if (whence == SEEK_END)
        base = (uint64_t)mf->size;
    else
        return (uint64_t)-1;

    uint64_t noff = base + off;
    if (noff > (uint64_t)((size_t)-1))
        return (uint64_t)-1;

    mf->off = (size_t)noff;
    return noff;
}

static int mem_close(thandle_t handle)
{
    (void)handle;
    return 0;
}

static uint64_t mem_size(thandle_t handle)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf)
        return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || !base || !size)
        return 0;
    *base = mf->buf;
    *size = (toff_t)mf->size;
    return 1;
}

static void mem_unmap(thandle_t handle, void *base, toff_t size)
{
    (void)handle;
    (void)base;
    (void)size;
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t maxlen)
{
    size_t len = (*n > 0) ? ((*p)[0] % (maxlen + 1)) : 0;
    if (*n > 0)
    {
        (*p)++;
        (*n)--;
    }
    if (len > *n)
        len = *n;
    char *s = (char *)malloc(len + 1);
    if (!s)
        return NULL;
    if (len > 0)
    {
        memcpy(s, *p, len);
        *p += len;
        *n -= len;
    }
    s[len] = '\0';
    return s;
}

int LLVMFuzzerTestOneInput_39(const uint8_t *Data, size_t Size)
{
    MemFile mf;
    TIFF *tif = NULL;
    const uint8_t *p = Data;
    size_t n = Size;

    memset(&mf, 0, sizeof(mf));

    if (Size > 0)
    {
        mf.buf = (uint8_t *)malloc(Size);
        if (!mf.buf)
            return 0;
        memcpy(mf.buf, Data, Size);
        mf.size = Size;
        mf.cap = Size;
    }

    {
        FILE *fp = fopen("./dummy_file", "wb");
        if (fp)
        {
            if (Size > 0)
                fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    {
        const char *mode = (n > 0 && (p[0] & 1)) ? "w" : "r";
        if (n > 0)
        {
            p++;
            n--;
        }

        tif = TIFFClientOpen("mem", mode, (thandle_t)&mf,
                             mem_read, mem_write, mem_seek, mem_close,
                             mem_size, mem_map, mem_unmap);
        if (!tif)
        {
            free(mf.buf);
            return 0;
        }
    }

    {
        thandle_t oldcd = TIFFSetClientdata(tif, (thandle_t)&mf);
        (void)oldcd;
        (void)TIFFClientdata(tif);

        if (n > 0)
        {
            thandle_t newcd = (n > 0 && (p[0] & 1)) ? (thandle_t)mf.buf : (thandle_t)Data;
            p++;
            n--;
            oldcd = TIFFSetClientdata(tif, newcd);
            (void)oldcd;
            (void)TIFFClientdata(tif);
            (void)TIFFSetClientdata(tif, (thandle_t)&mf);
        }
    }

    {
        int rounds = (n > 0) ? (p[0] % 8) : 0;
        int i;
        if (n > 0)
        {
            p++;
            n--;
        }

        for (i = 0; i < rounds; i++)
        {
            char *name = make_cstring(&p, &n, 32);
            if (!name)
                break;

            void *payload;
            if (n > 0 && (p[0] & 1))
            {
                payload = (void *)name;
                p++;
                n--;
            }
            else
            {
                payload = (void *)(uintptr_t)rd32(&p, &n);
            }

            TIFFSetClientInfo(tif, payload, name);
            (void)TIFFGetClientInfo(tif, name);

            if (n > 0 && (p[0] & 1))
            {
                p++;
                n--;
                TIFFSetClientInfo(tif, (void *)(uintptr_t)rd32(&p, &n), name);
                (void)TIFFGetClientInfo(tif, name);
            }

            free(name);
        }

        {
            char *missing = make_cstring(&p, &n, 16);
            if (missing)
            {
                (void)TIFFGetClientInfo(tif, missing);
                free(missing);
            }
            else
            {
                (void)TIFFGetClientInfo(tif, "nonexistent");
            }
        }
    }

    {
        uint32_t count = (n > 0) ? (uint32_t)(p[0] % 4) : 0;
        uint32_t i;
        TIFFFieldInfo *infos = NULL;

        if (n > 0)
        {
            p++;
            n--;
        }

        if (count > 0)
        {
            infos = (TIFFFieldInfo *)calloc(count, sizeof(TIFFFieldInfo));
            if (infos)
            {
                for (i = 0; i < count; i++)
                {
                    uint16_t tag = (uint16_t)(65000u + (rd16(&p, &n) % 500u));
                    int16_t rc = (int16_t)((rd16(&p, &n) % 5u) - 2);
                    int16_t wc = (int16_t)((rd16(&p, &n) % 5u) - 2);
                    int type = (int)(rd16(&p, &n) % 18u);
                    int bit = (int)(rd16(&p, &n) % 64u);
                    int ok = (n > 0) ? (p[0] & 1) : 0;
                    int pass = (n > 0) ? ((p[0] >> 1) & 1) : 0;
                    if (n > 0)
                    {
                        p++;
                        n--;
                    }

                    infos[i].field_tag = tag;
                    infos[i].field_readcount = rc;
                    infos[i].field_writecount = wc;
                    infos[i].field_type = (TIFFDataType)type;
                    infos[i].field_bit = (short)bit;
                    infos[i].field_oktochange = (unsigned char)ok;
                    infos[i].field_passcount = (unsigned char)pass;
                    infos[i].field_name = (char *)"fuzzfield";
                }

                (void)TIFFMergeFieldInfo(tif, infos, count);

                if (count > 1)
                    (void)TIFFMergeFieldInfo(tif, infos, count - 1);

                free(infos);
            }
        }
        else
        {
            (void)TIFFMergeFieldInfo(tif, NULL, 0);
        }
    }

    TIFFClose(tif);
    free(mf.buf);
    return 0;
}