// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFClientOpen at tif_open.c:71:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFFileName at tif_open.c:514:1 in tiffio.h
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
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#define CLOSE_FD _close
#define OPEN_FD _open
#else
#include <unistd.h>
#define CLOSE_FD close
#define OPEN_FD open
#endif

#include "tiffio.h"

typedef struct
{
    uint8_t *buf;
    size_t size;
    size_t pos;
} MemFile;

static tmsize_t mem_read(thandle_t handle, void *buf, tmsize_t size)
{
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || buf == NULL || size < 0)
        return 0;
    if (mf->pos > mf->size)
        return 0;
    size_t avail = mf->size - mf->pos;
    size_t want = (size_t)size;
    if (want > avail)
        want = avail;
    if (want > 0)
    {
        memcpy(buf, mf->buf + mf->pos, want);
        mf->pos += want;
    }
    return (tmsize_t)want;
}

static tmsize_t mem_write(thandle_t handle, void *buf, tmsize_t size)
{
    (void)handle;
    (void)buf;
    (void)size;
    return 0;
}

static uint64_t mem_seek(thandle_t handle, uint64_t off, int whence)
{
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL)
        return (uint64_t)-1;

    uint64_t newpos = 0;
    if (whence == SEEK_SET)
        newpos = off;
    else if (whence == SEEK_CUR)
        newpos = (uint64_t)mf->pos + off;
    else if (whence == SEEK_END)
        newpos = (uint64_t)mf->size + off;
    else
        return (uint64_t)-1;

    if (newpos > mf->size)
        newpos = mf->size;
    mf->pos = (size_t)newpos;
    return newpos;
}

static int mem_close(thandle_t handle)
{
    (void)handle;
    return 0;
}

static uint64_t mem_size(thandle_t handle)
{
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL)
        return 0;
    return (uint64_t)mf->size;
}

static int mem_map(thandle_t handle, void **base, toff_t *size)
{
    MemFile *mf = (MemFile *)handle;
    if (mf == NULL || base == NULL || size == NULL)
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

static void write_dummy_file(const uint8_t *Data, size_t Size)
{
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp == NULL)
        return;
    if (Size > 0)
        fwrite(Data, 1, Size, fp);
    fclose(fp);
}

static void build_string(char *dst, size_t dstsz, const uint8_t *src, size_t srcsz)
{
    size_t i;
    if (dstsz == 0)
        return;
    for (i = 0; i + 1 < dstsz && i < srcsz; ++i)
    {
        uint8_t c = src[i];
        dst[i] = (c >= 32 && c <= 126) ? (char)c : 'A';
    }
    dst[i] = '\0';
}

int LLVMFuzzerTestOneInput_59(const uint8_t *Data, size_t Size)
{
    static const char *modes[] = {
        "r", "r+", "w", "w8", "a", "rb", "wb", "rC", "rM", "invalid", ""
    };
    char custom_name1[64];
    char custom_name2[64];
    size_t mode_idx1 = 0, mode_idx2 = 0, mode_idx3 = 0;

    if (Data == NULL)
        return 0;

    write_dummy_file(Data, Size);

    if (Size > 0) mode_idx1 = Data[0] % (sizeof(modes) / sizeof(modes[0]));
    if (Size > 1) mode_idx2 = Data[1] % (sizeof(modes) / sizeof(modes[0]));
    if (Size > 2) mode_idx3 = Data[2] % (sizeof(modes) / sizeof(modes[0]));

    build_string(custom_name1, sizeof(custom_name1), Data, Size);
    build_string(custom_name2, sizeof(custom_name2), Data + (Size > 4 ? 4 : 0), Size > 4 ? Size - 4 : 0);

    {
        TIFF *tif = TIFFOpen("./dummy_file", modes[mode_idx1]);
        if (tif != NULL)
        {
            (void)TIFFFileName(tif);
            (void)TIFFFileno(tif);

            {
                const char *old1 = TIFFSetFileName(tif, custom_name1);
                (void)old1;
                (void)TIFFFileName(tif);

                const char *old2 = TIFFSetFileName(tif, custom_name2);
                (void)old2;
                (void)TIFFFileName(tif);

                const char *old3 = TIFFSetFileName(tif, NULL);
                (void)old3;
                (void)TIFFFileName(tif);

                (void)TIFFSetFileName(tif, "./dummy_file");
                (void)TIFFFileName(tif);
            }

            TIFFClose(tif);
        }
    }

    {
        int fd = OPEN_FD("./dummy_file", O_RDONLY
#ifdef O_BINARY
                         | O_BINARY
#endif
        );
        if (fd >= 0)
        {
            TIFF *tif_fd = TIFFFdOpen(fd, "./dummy_file", modes[mode_idx2]);
            if (tif_fd != NULL)
            {
                (void)TIFFFileno(tif_fd);
                (void)TIFFFileName(tif_fd);
                (void)TIFFSetFileName(tif_fd, custom_name1);
                (void)TIFFFileName(tif_fd);
                TIFFClose(tif_fd);
            }
            else
            {
                CLOSE_FD(fd);
            }
        }
    }

    {
        MemFile mf;
        TIFF *tif_client;

        mf.buf = (uint8_t *)Data;
        mf.size = Size;
        mf.pos = 0;

        tif_client = TIFFClientOpen(
            "memtiff",
            modes[mode_idx3],
            (thandle_t)&mf,
            mem_read,
            mem_write,
            mem_seek,
            mem_close,
            mem_size,
            mem_map,
            mem_unmap);

        if (tif_client != NULL)
        {
            (void)TIFFFileName(tif_client);
            (void)TIFFFileno(tif_client);
            (void)TIFFSetFileName(tif_client, custom_name2);
            (void)TIFFFileName(tif_client);
            (void)TIFFSetFileName(tif_client, NULL);
            (void)TIFFFileName(tif_client);
            TIFFClose(tif_client);
        }
    }

    return 0;
}