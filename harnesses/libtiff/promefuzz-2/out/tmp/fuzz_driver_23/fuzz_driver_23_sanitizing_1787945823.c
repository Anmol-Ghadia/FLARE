#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t *buf;
    size_t size;
    size_t pos;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf || !buf || size < 0)
        return 0;
    if (mf->pos > mf->size)
        return 0;
    size_t avail = mf->size - mf->pos;
    size_t n = (size_t)size;
    if (n > avail)
        n = avail;
    if (n > 0)
    {
        memcpy(buf, mf->buf + mf->pos, n);
        mf->pos += n;
    }
    return (tmsize_t)n;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    (void)buf;
    if (!mf || size < 0)
        return 0;
    if (mf->pos > mf->size)
        mf->pos = mf->size;
    size_t n = (size_t)size;
    if (n > mf->size - mf->pos)
        n = mf->size - mf->pos;
    mf->pos += n;
    return (tmsize_t)n;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence)
{
    MemFile *mf = (MemFile *)handle;
    if (!mf)
        return 0;

    size_t newpos = 0;
    if (whence == SEEK_SET)
    {
        newpos = (off > mf->size) ? mf->size : (size_t)off;
    }
    else if (whence == SEEK_CUR)
    {
        uint64_t cur = (uint64_t)mf->pos;
        uint64_t sum = cur + off;
        newpos = (sum > mf->size) ? mf->size : (size_t)sum;
    }
    else if (whence == SEEK_END)
    {
        newpos = (off > mf->size) ? 0 : (mf->size - (size_t)off);
    }
    else
    {
        newpos = mf->pos;
    }

    mf->pos = newpos;
    return (uint64_t)mf->pos;
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp)
    {
        if (Size > 0)
            fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    MemFile mf;
    mf.buf = (uint8_t *)(uintptr_t)Data;
    mf.size = Size;
    mf.pos = 0;

    const char *name = "mem";
    const char *mode = "r";
    if (Size > 0)
    {
        switch (Data[0] % 4)
        {
            case 0: mode = "r"; break;
            case 1: mode = "w"; break;
            case 2: mode = "a"; break;
            default: mode = "r+"; break;
        }
    }

    TIFF *tif = TIFFClientOpen(name, mode, (thandle_t)&mf,
                               mem_read, mem_write, mem_seek, mem_close,
                               mem_size, mem_map, mem_unmap);
    if (!tif)
        return 0;

    (void)TIFFFileName(tif);
    (void)TIFFFileno(tif);
    (void)TIFFLastDirectory(tif);

    int mode_vals[4];
    mode_vals[0] = (Size > 1) ? (int)(int8_t)Data[1] : 0;
    mode_vals[1] = (Size > 2) ? (int)Data[2] : -1;
    mode_vals[2] = (Size > 3) ? ((int)Data[3] << 8) | ((Size > 4) ? Data[4] : 0) : 1;
    mode_vals[3] = 0x7fffffff;

    for (int i = 0; i < 4; i++)
    {
        int old_mode = TIFFSetMode(tif, mode_vals[i]);
        (void)old_mode;
        (void)TIFFFileno(tif);
        (void)TIFFFileName(tif);
        (void)TIFFLastDirectory(tif);
    }

    int fd_vals[4];
    fd_vals[0] = -1;
    fd_vals[1] = 0;
    fd_vals[2] = 1;
    fd_vals[3] = (Size > 5) ? (int)((Data[5] << 8) | ((Size > 6) ? Data[6] : 0)) : 1234;

    for (int i = 0; i < 4; i++)
    {
        int old_fd = TIFFSetFileno(tif, fd_vals[i]);
        (void)old_fd;
        (void)TIFFFileno(tif);
        (void)TIFFFileName(tif);
        (void)TIFFLastDirectory(tif);
    }

    TIFFClose(tif);
    return 0;
}