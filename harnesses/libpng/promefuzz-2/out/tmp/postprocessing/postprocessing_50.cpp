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

static void WriteDataToDummyFile(const uint8_t *data, size_t size) {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (size > 0) std::fwrite(data, 1, size, fp);
        std::fclose(fp);
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    WriteDataToDummyFile(Data, Size);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return 0;
    }

    int v1 = static_cast<int>(ReadU32(Data, Size, 0));
    int v2 = static_cast<int>(ReadU32(Data, Size, 4));
    int v3 = static_cast<int>(ReadU32(Data, Size, 8));
    int v4 = static_cast<int>(ReadU32(Data, Size, 12));
    int v5 = static_cast<int>(ReadU32(Data, Size, 16));
    int v6 = static_cast<int>(ReadU32(Data, Size, 20));

    png_set_text_compression_method(png_ptr, v1);
    png_set_compression_window_bits(png_ptr, v2);
    png_set_text_compression_mem_level(png_ptr, v3);
    png_set_text_compression_level(png_ptr, v4);
    png_set_compression_level(png_ptr, v5);
    png_set_text_compression_strategy(png_ptr, v6);

    png_set_text_compression_method(png_ptr, 8);
    png_set_text_compression_method(png_ptr, -1);
    png_set_compression_window_bits(png_ptr, 8);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_window_bits(png_ptr, -100);
    png_set_compression_window_bits(png_ptr, 100);
    png_set_text_compression_mem_level(png_ptr, 0);
    png_set_text_compression_mem_level(png_ptr, 9);
    png_set_text_compression_level(png_ptr, -1);
    png_set_text_compression_level(png_ptr, 0);
    png_set_text_compression_level(png_ptr, 9);
    png_set_compression_level(png_ptr, -1);
    png_set_compression_level(png_ptr, 0);
    png_set_compression_level(png_ptr, 9);
    png_set_text_compression_strategy(png_ptr, 0);
    png_set_text_compression_strategy(png_ptr, 1);
    png_set_text_compression_strategy(png_ptr, 2);
    png_set_text_compression_strategy(png_ptr, 3);
    png_set_text_compression_strategy(png_ptr, 4);

    png_set_text_compression_method(nullptr, v1);
    png_set_compression_window_bits(nullptr, v2);
    png_set_text_compression_mem_level(nullptr, v3);
    png_set_text_compression_level(nullptr, v4);
    png_set_compression_level(nullptr, v5);
    png_set_text_compression_strategy(nullptr, v6);

    for (size_t i = 0; i < Size; ++i) {
        int value = static_cast<int>(static_cast<int8_t>(Data[i]));
        switch (i % 6) {
            case 0:
                png_set_text_compression_method(png_ptr, value);
                break;
            case 1:
                png_set_compression_window_bits(png_ptr, value);
                break;
            case 2:
                png_set_text_compression_mem_level(png_ptr, value);
                break;
            case 3:
                png_set_text_compression_level(png_ptr, value);
                break;
            case 4:
                png_set_compression_level(png_ptr, value);
                break;
            case 5:
                png_set_text_compression_strategy(png_ptr, value);
                break;
        }
    }

    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
}