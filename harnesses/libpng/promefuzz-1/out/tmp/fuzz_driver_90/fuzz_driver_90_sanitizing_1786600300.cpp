#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
    if (png_ptr == nullptr || data == nullptr) return;
    FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
    if (fp != nullptr) {
        (void)fwrite(data, 1, length, fp);
    }
}

static void PngFlushCallback(png_structp png_ptr) {
    if (png_ptr == nullptr) return;
    FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
    if (fp != nullptr) {
        (void)fflush(fp);
    }
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t* Data, size_t Size) {
    png_uint_32 v = 0;
    size_t n = Size < 4 ? Size : 4;
    for (size_t i = 0; i < n; ++i) {
        v = (v << 8) | static_cast<png_uint_32>(Data[i]);
    }
    return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t buf32[4] = {0, 0, 0, 0};
    uint8_t buf16[2] = {0, 0};

    png_uint_32 v32 = ReadU32(Data, Size);
    png_save_uint_32(buf32, v32);

    unsigned int v16in = 0;
    if (Size >= 2) {
        v16in = (static_cast<unsigned int>(Data[0]) << 8) |
                static_cast<unsigned int>(Data[1]);
    } else if (Size == 1) {
        v16in = Data[0];
    }
    png_save_uint_16(buf16, v16in);
    (void)png_get_uint_16(buf16);

    FILE* fp = fopen("./dummy_file", "wb");
    if (fp == nullptr) {
        return 0;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                  PngErrorCallback, PngWarningCallback);
    if (png_ptr == nullptr) {
        fclose(fp);
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 0;
    }

    png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

    png_write_sig(png_ptr);

    png_byte chunk_name1[5] = {'t', 'E', 'S', 'T', 0};
    png_byte chunk_name2[5] = {'I', 'D', 'A', 'T', 0};
    png_byte chunk_name3[5] = {'a', 'b', 'c', 'd', 0};
    png_byte chunk_name4[5] = {'\0', '\0', '\0', '\0', 0};

    png_byte dynamic_name[5] = {'A', 'A', 'A', 'A', 0};
    for (size_t i = 0; i < 4 && i < Size; ++i) {
        dynamic_name[i] = static_cast<png_byte>(Data[i]);
    }

    const png_byte* payload = reinterpret_cast<const png_byte*>(Data);
    png_uint_32 len1 = static_cast<png_uint_32>(Size > 64 ? 64 : Size);
    png_uint_32 len2 = static_cast<png_uint_32>(Size > 16 ? 16 : Size);
    png_uint_32 len3 = static_cast<png_uint_32>(Size > 4 ? 4 : Size);
    png_uint_32 len4 = static_cast<png_uint_32>(Size > 0 ? (v32 & 0xFFu) : 0u);
    if (len4 > Size) len4 = static_cast<png_uint_32>(Size);

    png_write_chunk_start(png_ptr, chunk_name1, len1);
    png_write_chunk(png_ptr, chunk_name1, len1 > 0 ? payload : nullptr, len1);

    png_write_chunk_start(png_ptr, chunk_name2, len2);
    png_write_chunk(png_ptr, chunk_name2, len2 > 0 ? payload : nullptr, len2);

    png_write_chunk_start(png_ptr, dynamic_name, len3);
    png_write_chunk(png_ptr, dynamic_name, len3 > 0 ? payload : nullptr, len3);

    png_write_chunk_start(png_ptr, chunk_name3, len4);
    png_write_chunk(png_ptr, chunk_name3, len4 > 0 ? payload : nullptr, len4);

    png_write_chunk_start(png_ptr, chunk_name4, 0);
    png_write_chunk(png_ptr, chunk_name4, nullptr, 0);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}