#ifndef TIFF_FUZZING_H
#define TIFF_FUZZING_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <tiffio.h>

typedef struct
{
    const uint8_t *data;
    uint64_t size;
    uint64_t offset;
} TIFFFuzzBuffer;

static void TIFFFuzzSilentHandler(const char *module, const char *fmt,
                                  va_list ap)
{
    (void)module;
    (void)fmt;
    (void)ap;
}

static tmsize_t TIFFFuzzRead(thandle_t handle, void *buf, tmsize_t count)
{
    TIFFFuzzBuffer *mem = (TIFFFuzzBuffer *)handle;
    size_t amount;
    uint64_t remaining;

    if (mem == NULL || buf == NULL || count <= 0 || mem->data == NULL)
        return 0;
    if (mem->offset >= mem->size)
        return 0;

    remaining = mem->size - mem->offset;
    amount = (size_t)count;
    if ((uint64_t)amount > remaining)
        amount = (size_t)remaining;

    memcpy(buf, mem->data + mem->offset, amount);
    mem->offset += amount;
    return (tmsize_t)amount;
}

static tmsize_t TIFFFuzzWrite(thandle_t handle, void *buf, tmsize_t count)
{
    (void)handle;
    (void)buf;
    (void)count;
    return 0;
}

static toff_t TIFFFuzzSeek(thandle_t handle, toff_t off, int whence)
{
    TIFFFuzzBuffer *mem = (TIFFFuzzBuffer *)handle;
    uint64_t next;

    if (mem == NULL)
        return (toff_t)-1;

    switch (whence)
    {
        case SEEK_SET:
            next = off;
            break;
        case SEEK_CUR:
            if (UINT64_MAX - mem->offset < off)
                return (toff_t)-1;
            next = mem->offset + off;
            break;
        case SEEK_END:
            if (UINT64_MAX - mem->size < off)
                return (toff_t)-1;
            next = mem->size + off;
            break;
        default:
            return (toff_t)-1;
    }

    mem->offset = next;
    return (toff_t)next;
}

static int TIFFFuzzClose(thandle_t handle)
{
    (void)handle;
    return 0;
}

static toff_t TIFFFuzzSize(thandle_t handle)
{
    TIFFFuzzBuffer *mem = (TIFFFuzzBuffer *)handle;

    if (mem == NULL)
        return 0;
    return (toff_t)mem->size;
}

static TIFF *TIFFFuzzOpen(const uint8_t *data, size_t size, TIFFFuzzBuffer *mem)
{
    if (mem == NULL)
        return NULL;

    mem->data = data;
    mem->size = (uint64_t)size;
    mem->offset = 0;

    TIFFSetErrorHandler(TIFFFuzzSilentHandler);
    TIFFSetWarningHandler(TIFFFuzzSilentHandler);

    /* Keep the name constant so only the file contents are fuzzed. */
    return TIFFClientOpen("mem.tif", "rm", (thandle_t)mem, TIFFFuzzRead,
                          TIFFFuzzWrite, TIFFFuzzSeek, TIFFFuzzClose,
                          TIFFFuzzSize, NULL, NULL);
}

static int TIFFFuzzMulU64(uint64_t left, uint64_t right, uint64_t *product)
{
    if (product == NULL)
        return 0;
    if (left == 0 || right == 0)
    {
        *product = 0;
        return 1;
    }
    if (left > UINT64_MAX / right)
        return 0;

    *product = left * right;
    return 1;
}

static int TIFFFuzzIsAllocatable(uint64_t size, uint64_t max_size)
{
    return size != 0 && size <= max_size && size <= (uint64_t)TIFF_TMSIZE_T_MAX;
}

static void *TIFFFuzzMalloc(uint64_t size, uint64_t max_size)
{
    if (!TIFFFuzzIsAllocatable(size, max_size))
        return NULL;
    return _TIFFmalloc((tmsize_t)size);
}

#endif
