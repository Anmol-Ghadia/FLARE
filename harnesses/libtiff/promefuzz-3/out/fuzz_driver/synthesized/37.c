// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFSetClientInfo at tif_extension.c:78:6 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFGetClientInfo at tif_extension.c:64:7 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFMergeFieldInfo at tif_dirinfo.c:1062:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
// TIFFFreeDirectory at tif_dir.c:1306:1 in tiffio.h
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

typedef struct
{
    uint8_t *buf;
    size_t size;
    size_t cap;
    toff_t off;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0)
        return 0;
    if ((size_t)mf->off >= mf->size)
        return 0;
    size_t avail = mf->size - (size_t)mf->off;
    size_t n = (size_t)size;
    if (n > avail)
        n = avail;
    memcpy(buf, mf->buf + mf->off, n);
    mf->off += (toff_t)n;
    return (tmsize_t)n;
}

static int mem_grow(MemFile *mf, size_t need)
{
    if (need <= mf->cap)
        return 1;
    size_t newcap = mf->cap ? mf->cap : 64;
    while (newcap < need)
    {
        if (newcap > ((size_t)-1) / 2)
        {
            newcap = need;
            break;
        }
        newcap *= 2;
    }
    uint8_t *p = (uint8_t *)realloc(mf->buf, newcap);
    if (!p)
        return 0;
    if (newcap > mf->cap)
        memset(p + mf->cap, 0, newcap - mf->cap);
    mf->buf = p;
    mf->cap = newcap;
    return 1;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0)
        return 0;
    size_t n = (size_t)size;
    size_t end = (size_t)mf->off + n;
    if (end < (size_t)mf->off)
        return 0;
    if (!mem_grow(mf, end))
        return 0;
    memcpy(mf->buf + mf->off, buf, n);
    mf->off += (toff_t)n;
    if (end > mf->size)
        mf->size = end;
    return (tmsize_t)n;
}

static toff_t mem_seek(thandle_t handle, toff_t off, int whence)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf)
        return (toff_t)-1;

    toff_t base = 0;
    if (whence == SEEK_SET)
        base = 0;
    else if (whence == SEEK_CUR)
        base = mf->off;
    else if (whence == SEEK_END)
        base = (toff_t)mf->size;
    else
        return (toff_t)-1;

    if (off > 0 && base > ((toff_t)-1) - off)
        return (toff_t)-1;

    toff_t noff = base + off;
    mf->off = noff;
    return mf->off;
}

static int mem_close(thandle_t handle)
{
    (void)handle;
    return 0;
}

static toff_t mem_size(thandle_t handle)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf)
        return 0;
    return (toff_t)mf->size;
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

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int LLVMFuzzerTestOneInput_37(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp)
    {
        if (Size)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    MemFile mf;
    memset(&mf, 0, sizeof(mf));

    if (Size)
    {
        mf.buf = (uint8_t *)malloc(Size);
        if (!mf.buf)
            return 0;
        memcpy(mf.buf, Data, Size);
        mf.size = Size;
        mf.cap = Size;
    }

    const char *mode = "r";
    if (Size > 0)
    {
        switch (Data[0] % 4)
        {
            case 0: mode = "r"; break;
            case 1: mode = "w"; break;
            case 2: mode = "r+"; break;
            default: mode = "w8"; break;
        }
    }

    TIFF *tif = TIFFClientOpen(
        "mem",
        mode,
        (thandle_t)&mf,
        mem_read,
        mem_write,
        mem_seek,
        mem_close,
        mem_size,
        mem_map,
        mem_unmap);

    if (!tif)
    {
        free(mf.buf);
        return 0;
    }

    char *name1 = (char *)malloc(32);
    char *name2 = (char *)malloc(32);
    char *name3 = (char *)malloc(32);
    void *payload1 = NULL;
    void *payload2 = NULL;
    void *payload3 = NULL;

    if (name1) snprintf(name1, 32, "k1_%02x", Size ? Data[0] : 0);
    if (name2) snprintf(name2, 32, "k2_%02x", Size > 1 ? Data[1] : 0);
    if (name3) snprintf(name3, 32, "k3_%02x", Size > 2 ? Data[2] : 0);

    payload1 = malloc(16);
    payload2 = malloc(24);
    payload3 = malloc(8);

    if (payload1 && Size)
        memcpy(payload1, Data, Size < 16 ? Size : 16);
    if (payload2 && Size > 1)
        memcpy(payload2, Data + 1, (Size - 1) < 24 ? (Size - 1) : 24);
    if (payload3 && Size > 2)
        memcpy(payload3, Data + 2, (Size - 2) < 8 ? (Size - 2) : 8);

    if (name1)
        TIFFSetClientInfo(tif, payload1, name1);
    if (name2)
        TIFFSetClientInfo(tif, payload2, name2);
    if (name1)
        TIFFSetClientInfo(tif, payload3, name1);

    if (name1)
        (void)TIFFGetClientInfo(tif, name1);
    if (name2)
        (void)TIFFGetClientInfo(tif, name2);
    if (name3)
        (void)TIFFGetClientInfo(tif, name3);
    (void)TIFFGetClientInfo(tif, "");
    (void)TIFFGetClientInfo(tif, "nonexistent");

    char *field_name1 = (char *)malloc(64);
    char *field_name2 = (char *)malloc(64);
    char *field_name3 = (char *)malloc(64);

    if (field_name1) snprintf(field_name1, 64, "FuzzFieldA_%u", Size > 3 ? rd16(Data + 2) : 1U);
    if (field_name2) snprintf(field_name2, 64, "FuzzFieldB_%u", Size > 5 ? rd16(Data + 4) : 2U);
    if (field_name3) snprintf(field_name3, 64, "FuzzFieldC_%u", Size > 7 ? rd16(Data + 6) : 3U);

    TIFFFieldInfo finfo[3];
    memset(finfo, 0, sizeof(finfo));

    finfo[0].field_tag = 65000U + (Size > 3 ? (rd16(Data + 2) % 100) : 0);
    finfo[0].field_readcount = TIFF_VARIABLE2;
    finfo[0].field_writecount = TIFF_VARIABLE2;
    finfo[0].field_type = (TIFFDataType)((Size > 8 ? Data[8] : 0) % 18);
    finfo[0].field_bit = FIELD_CUSTOM;
    finfo[0].field_oktochange = 1;
    finfo[0].field_passcount = (Size > 9 ? Data[9] & 1 : 0);
    finfo[0].field_name = field_name1;

    finfo[1].field_tag = 65100U + (Size > 11 ? (rd16(Data + 10) % 100) : 0);
    finfo[1].field_readcount = 1;
    finfo[1].field_writecount = 1;
    finfo[1].field_type = (TIFFDataType)((Size > 12 ? Data[12] : 1) % 18);
    finfo[1].field_bit = FIELD_CUSTOM;
    finfo[1].field_oktochange = (Size > 13 ? Data[13] & 1 : 1);
    finfo[1].field_passcount = (Size > 14 ? Data[14] & 1 : 0);
    finfo[1].field_name = field_name2;

    finfo[2].field_tag = 65200U + (Size > 17 ? (rd16(Data + 15) % 100) : 0);
    finfo[2].field_readcount = -1;
    finfo[2].field_writecount = -1;
    finfo[2].field_type = (TIFFDataType)((Size > 18 ? Data[18] : 2) % 18);
    finfo[2].field_bit = FIELD_CUSTOM;
    finfo[2].field_oktochange = 1;
    finfo[2].field_passcount = 1;
    finfo[2].field_name = field_name3;

    (void)TIFFMergeFieldInfo(tif, finfo, 1);
    (void)TIFFMergeFieldInfo(tif, finfo, 2);
    (void)TIFFMergeFieldInfo(tif, finfo, 3);

    if (Size > 20 && (Data[20] & 1))
        TIFFFreeDirectory(tif);
    if (Size > 21 && (Data[21] & 1))
        TIFFFreeDirectory(tif);

    TIFFCleanup(tif);

    free(field_name1);
    free(field_name2);
    free(field_name3);
    free(name1);
    free(name2);
    free(name3);
    free(payload1);
    free(payload2);
    free(payload3);
    free(mf.buf);

    return 0;
}