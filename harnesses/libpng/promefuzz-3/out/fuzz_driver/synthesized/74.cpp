// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_option at png.c:4332:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
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
         static_cast<uint32_t>(data[offset + 3]);
}

static void UserWarningFn(png_structp, png_const_charp) {
}

static void UserErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

extern "C" int LLVMFuzzerTestOneInput_74(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, UserErrorFn, UserWarningFn);
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

  int method1 = static_cast<int>(ReadU32(Data, Size, 0));
  int window_bits1 = static_cast<int>(ReadU32(Data, Size, 4));
  int option1 = static_cast<int>(ReadU32(Data, Size, 8));
  int onoff1 = static_cast<int>(ReadU32(Data, Size, 12));
  int text_method1 = static_cast<int>(ReadU32(Data, Size, 16));
  int text_strategy1 = static_cast<int>(ReadU32(Data, Size, 20));
  int strategy1 = static_cast<int>(ReadU32(Data, Size, 24));

  png_set_compression_method(png_ptr, method1);
  png_set_compression_window_bits(png_ptr, window_bits1);
  (void)png_set_option(png_ptr, option1, onoff1);
  png_set_text_compression_method(png_ptr, text_method1);
  png_set_text_compression_strategy(png_ptr, text_strategy1);
  png_set_compression_strategy(png_ptr, strategy1);

  for (size_t i = 0; i < Size; ++i) {
    int selector = Data[i] % 6;
    int value = static_cast<int>(static_cast<int8_t>(Data[i]));
    switch (selector) {
      case 0:
        png_set_compression_method(png_ptr, value);
        break;
      case 1:
        png_set_compression_window_bits(png_ptr, value);
        break;
      case 2: {
        int option = value;
        int onoff = (i + 1 < Size) ? static_cast<int>(Data[i + 1]) : 0;
        (void)png_set_option(png_ptr, option, onoff);
        break;
      }
      case 3:
        png_set_text_compression_method(png_ptr, value);
        break;
      case 4:
        png_set_text_compression_strategy(png_ptr, value);
        break;
      case 5:
        png_set_compression_strategy(png_ptr, value);
        break;
    }
  }

  png_set_compression_method(nullptr, method1);
  png_set_compression_window_bits(nullptr, window_bits1);
  (void)png_set_option(nullptr, option1, onoff1);
  png_set_text_compression_method(nullptr, text_method1);
  png_set_text_compression_strategy(nullptr, text_strategy1);
  png_set_compression_strategy(nullptr, strategy1);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}