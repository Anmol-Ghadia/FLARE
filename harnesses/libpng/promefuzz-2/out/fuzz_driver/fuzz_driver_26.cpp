// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <png.h>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
    if (offset + 4 > size) return 0;
    return (static_cast<uint32_t>(data[offset]) << 24) |
           (static_cast<uint32_t>(data[offset + 1]) << 16) |
           (static_cast<uint32_t>(data[offset + 2]) << 8) |
           (static_cast<uint32_t>(data[offset + 3]));
}

static int ReadS32(const uint8_t *data, size_t size, size_t offset) {
    return static_cast<int>(ReadU32(data, size, offset));
}

static void PngErrorFn(png_structp, png_const_charp) {
    // Prevent libpng from aborting the process on fatal errors.
}

static void PngWarningFn(png_structp, png_const_charp) {
    // Ignore warnings; fuzzing is interested in state exploration.
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            std::fwrite(Data, 1, Size, fp);
        }
        std::fclose(fp);
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  nullptr,
                                                  PngErrorFn,
                                                  PngWarningFn);
    if (png_ptr == nullptr) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    int v0 = ReadS32(Data, Size, 0);
    int v1 = ReadS32(Data, Size, 4);
    int v2 = ReadS32(Data, Size, 8);
    int v3 = ReadS32(Data, Size, 12);
    int v4 = ReadS32(Data, Size, 16);
    int v5 = ReadS32(Data, Size, 20);

    png_structp maybe_null = (Size > 24 && (Data[24] & 1)) ? nullptr : png_ptr;

    png_set_text_compression_method(maybe_null, v0);
    png_set_compression_strategy(maybe_null, v1);
    png_set_compression_window_bits(maybe_null, v2);
    png_set_compression_mem_level(maybe_null, v3);
    png_set_text_compression_window_bits(maybe_null, v4);
    png_set_text_compression_level(maybe_null, v5);

    png_set_text_compression_method(png_ptr, 8);
    png_set_text_compression_method(png_ptr, -1);
    png_set_text_compression_method(png_ptr, 0);
    png_set_text_compression_method(png_ptr, 9);

    png_set_compression_strategy(png_ptr, 0);
    png_set_compression_strategy(png_ptr, 1);
    png_set_compression_strategy(png_ptr, 2);
    png_set_compression_strategy(png_ptr, -1);
    png_set_compression_strategy(png_ptr, 999);

    png_set_compression_window_bits(png_ptr, 8);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_window_bits(png_ptr, 7);
    png_set_compression_window_bits(png_ptr, 16);
    png_set_compression_window_bits(png_ptr, v2);

    png_set_compression_mem_level(png_ptr, 1);
    png_set_compression_mem_level(png_ptr, 9);
    png_set_compression_mem_level(png_ptr, 0);
    png_set_compression_mem_level(png_ptr, -1);
    png_set_compression_mem_level(png_ptr, v3);

    png_set_text_compression_window_bits(png_ptr, 8);
    png_set_text_compression_window_bits(png_ptr, 15);
    png_set_text_compression_window_bits(png_ptr, 7);
    png_set_text_compression_window_bits(png_ptr, 16);
    png_set_text_compression_window_bits(png_ptr, v4);

    png_set_text_compression_level(png_ptr, -1);
    png_set_text_compression_level(png_ptr, 0);
    png_set_text_compression_level(png_ptr, 1);
    png_set_text_compression_level(png_ptr, 9);
    png_set_text_compression_level(png_ptr, v5);

    if (Size > 25) {
        for (size_t i = 25; i < Size; ++i) {
            int selector = Data[i] % 6;
            int value = static_cast<int>(static_cast<int8_t>(Data[i]));
            switch (selector) {
                case 0:
                    png_set_text_compression_method(png_ptr, value);
                    break;
                case 1:
                    png_set_compression_strategy(png_ptr, value);
                    break;
                case 2:
                    png_set_compression_window_bits(png_ptr, value);
                    break;
                case 3:
                    png_set_compression_mem_level(png_ptr, value);
                    break;
                case 4:
                    png_set_text_compression_window_bits(png_ptr, value);
                    break;
                case 5:
                    png_set_text_compression_level(png_ptr, value);
                    break;
            }
        }
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}