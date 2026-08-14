// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_write_sig at pngwutil.c:51:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
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
#include <vector>
#include <cstdio>
#include <cstring>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
    std::vector<unsigned char>* out =
        reinterpret_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
    if (out != nullptr && data != nullptr && length > 0) {
        out->insert(out->end(), data, data + length);
    }
}

static void PngFlushCallback(png_structp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == nullptr) return 0;

    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) std::fwrite(Data, 1, Size, fp);
        std::fclose(fp);
    }

    unsigned char buf32[4] = {0};
    unsigned char buf16[2] = {0};

    png_uint_32 v32 = 0;
    if (Size >= 4) {
        v32 = (static_cast<png_uint_32>(Data[0]) << 24) |
              (static_cast<png_uint_32>(Data[1]) << 16) |
              (static_cast<png_uint_32>(Data[2]) << 8)  |
              (static_cast<png_uint_32>(Data[3]));
    } else {
        for (size_t i = 0; i < Size; ++i) {
            v32 = (v32 << 8) | Data[i];
        }
    }

    png_save_uint_32(buf32, v32);
    png_uint_16 v16 = png_get_uint_16(Size >= 2 ? Data : buf16);
    png_save_uint_16(buf16, static_cast<unsigned int>(v16));
    png_get_uint_16(buf16);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) return 0;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    std::vector<unsigned char> out;
    png_set_write_fn(png_ptr, &out, PngWriteCallback, PngFlushCallback);

    if (Size > 0) {
        png_set_sig_bytes(png_ptr, static_cast<int>(Data[0] % 8));
    } else {
        png_set_sig_bytes(png_ptr, 0);
    }

    png_write_sig(png_ptr);

    unsigned char chunk_name[4] = {'t', 'E', 'S', 'T'};
    if (Size >= 8) {
        for (int i = 0; i < 4; ++i) {
            unsigned char c = Data[i];
            if (c < 32 || c > 126) c = static_cast<unsigned char>('A' + (c % 26));
            chunk_name[i] = c;
        }
    }

    size_t payload_offset = Size > 4 ? 4 : Size;
    size_t remaining = Size > payload_offset ? (Size - payload_offset) : 0;

    png_uint_32 len1 = static_cast<png_uint_32>(remaining > 1024 ? 1024 : remaining);
    png_write_chunk_start(png_ptr, chunk_name, len1);

    png_const_bytep payload = remaining ? (Data + payload_offset) : nullptr;
    png_write_chunk(png_ptr, chunk_name, payload, len1);

    png_uint_32 len2 = 0;
    if (Size >= 12) {
        len2 = static_cast<png_uint_32>(((Data[4] << 8) | Data[5]) % 64);
    }
    png_write_chunk_start(png_ptr, reinterpret_cast<png_const_bytep>("IEND"), len2);
    png_write_chunk(png_ptr, reinterpret_cast<png_const_bytep>("IEND"), payload, len2 <= remaining ? len2 : static_cast<png_uint_32>(remaining));

    png_write_chunk(png_ptr, reinterpret_cast<png_const_bytep>("tEXt"), reinterpret_cast<png_const_bytep>(buf32), sizeof(buf32));
    png_write_chunk(png_ptr, reinterpret_cast<png_const_bytep>("gAMA"), reinterpret_cast<png_const_bytep>(buf16), sizeof(buf16));

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}