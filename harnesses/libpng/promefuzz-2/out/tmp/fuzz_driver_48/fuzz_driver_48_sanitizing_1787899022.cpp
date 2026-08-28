#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>

static int g_pause_mode = 0;

static int UserChunkCallback(png_structp png_ptr, png_unknown_chunkp chunk)
{
    if (png_ptr != nullptr)
    {
        png_voidp p1 = png_get_progressive_ptr(png_ptr);
        png_voidp p2 = png_get_io_ptr(png_ptr);
        (void)p1;
        (void)p2;

        if (chunk != nullptr)
        {
            if ((chunk->size & 1u) != 0)
                return 1;
            if ((chunk->size & 2u) != 0)
                return 0;
        }

        if (g_pause_mode == 1)
            (void)png_process_data_pause(png_ptr, 1);
        else if (g_pause_mode == 2)
            (void)png_process_data_pause(png_ptr, 0);
    }

    return 0;
}

static void InfoCallback(png_structp png_ptr, png_infop info_ptr)
{
    (void)info_ptr;
    png_voidp p1 = png_get_progressive_ptr(png_ptr);
    png_voidp p2 = png_get_io_ptr(png_ptr);
    (void)p1;
    (void)p2;

    if (g_pause_mode == 3)
        (void)png_process_data_pause(png_ptr, 1);
    else if (g_pause_mode == 4)
        (void)png_process_data_pause(png_ptr, 0);
}

static void RowCallback(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
    (void)new_row;
    (void)row_num;
    (void)pass;

    png_voidp p1 = png_get_progressive_ptr(png_ptr);
    png_voidp p2 = png_get_io_ptr(png_ptr);
    (void)p1;
    (void)p2;

    if (g_pause_mode == 5)
        (void)png_process_data_pause(png_ptr, 1);
    else if (g_pause_mode == 6)
        (void)png_process_data_pause(png_ptr, 0);
}

static void EndCallback(png_structp png_ptr, png_infop info_ptr)
{
    (void)info_ptr;
    png_voidp p1 = png_get_progressive_ptr(png_ptr);
    png_voidp p2 = png_get_io_ptr(png_ptr);
    (void)p1;
    (void)p2;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    {
        std::ofstream ofs("./dummy_file", std::ios::binary);
        if (ofs)
            ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }

    png_structp null_png = nullptr;
    (void)png_get_progressive_ptr(null_png);
    (void)png_get_io_ptr(null_png);
    png_set_read_user_chunk_fn(null_png, nullptr, UserChunkCallback);
    png_set_progressive_read_fn(null_png, nullptr, InfoCallback, RowCallback, EndCallback);
    (void)png_process_data_pause(null_png, 0);
    png_process_data(null_png, nullptr, nullptr, 0);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr)
        return 0;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    std::vector<uint8_t> user_ctx;
    size_t ctx_size = Size > 32 ? 32 : Size;
    user_ctx.assign(Data, Data + ctx_size);

    png_set_read_user_chunk_fn(png_ptr, user_ctx.empty() ? nullptr : user_ctx.data(), UserChunkCallback);
    png_set_progressive_read_fn(
        png_ptr,
        user_ctx.empty() ? nullptr : user_ctx.data(),
        InfoCallback,
        RowCallback,
        EndCallback);

    (void)png_get_progressive_ptr(png_ptr);
    (void)png_get_io_ptr(png_ptr);

    if (Size == 0)
    {
        png_process_data(png_ptr, info_ptr, nullptr, 0);
    }
    else
    {
        size_t offset = 0;
        uint8_t mode = Data[0] % 8;
        g_pause_mode = mode;

        while (offset < Size)
        {
            size_t remaining = Size - offset;
            size_t chunk = 1;

            switch ((Data[offset] ^ mode) % 5)
            {
                case 0: chunk = 1; break;
                case 1: chunk = remaining < 2 ? remaining : 2; break;
                case 2: chunk = remaining < 8 ? remaining : 8; break;
                case 3: chunk = remaining < 64 ? remaining : 64; break;
                default: chunk = remaining; break;
            }

            png_process_data(png_ptr, info_ptr,
                const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + offset)),
                chunk);

            offset += chunk;

            if ((offset % 3) == 0)
            {
                (void)png_get_progressive_ptr(png_ptr);
                (void)png_get_io_ptr(png_ptr);
            }
        }

        if ((mode & 1u) != 0)
        {
            png_process_data(png_ptr, info_ptr,
                const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)),
                0);
        }
    }

    (void)png_get_progressive_ptr(png_ptr);
    (void)png_get_io_ptr(png_ptr);

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
}