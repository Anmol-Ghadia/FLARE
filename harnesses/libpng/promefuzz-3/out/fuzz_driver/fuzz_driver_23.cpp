// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_rgb_to_gray_fixed at pngrtran.c:970:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_rgb_to_gray_fixed at pngrtran.c:970:1 in png.h
// png_get_rgb_to_gray_status at pngget.c:1154:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

static png_fixed_point ReadFixed(const uint8_t* data, size_t size, size_t& off) {
  if (off + 4 > size) return 1;
  png_int_32 v = (png_int_32)(
      ((png_uint_32)data[off] << 24) |
      ((png_uint_32)data[off + 1] << 16) |
      ((png_uint_32)data[off + 2] << 8) |
      ((png_uint_32)data[off + 3]));
  off += 4;
  return v;
}

static int ReadInt(const uint8_t* data, size_t size, size_t& off) {
  if (off + 4 > size) return 0;
  int v = (int)(
      ((unsigned)data[off] << 24) |
      ((unsigned)data[off + 1] << 16) |
      ((unsigned)data[off + 2] << 8) |
      ((unsigned)data[off + 3]));
  off += 4;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  size_t off = 0;

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  if (Size > 0) background.index = Data[off++ % Size];
  background.red = (png_uint_16)(ReadInt(Data, Size, off) & 0xFFFF);
#if defined(PNG_READ_BACKGROUND_SUPPORTED)
  background.green = (png_uint_16)(ReadInt(Data, Size, off) & 0xFFFF);
  background.blue = (png_uint_16)(ReadInt(Data, Size, off) & 0xFFFF);
  background.gray = (png_uint_16)(ReadInt(Data, Size, off) & 0xFFFF);
#endif

  png_fixed_point screen_gamma = ReadFixed(Data, Size, off);
  png_fixed_point override_file_gamma = ReadFixed(Data, Size, off);
  png_fixed_point red = ReadFixed(Data, Size, off);
  png_fixed_point green = ReadFixed(Data, Size, off);
  int error_action = ReadInt(Data, Size, off);
  int background_gamma_code = ReadInt(Data, Size, off);
  int need_expand = ReadInt(Data, Size, off);

  if ((Size > 0 ? Data[0] : 0) & 1) {
    png_set_strip_16(png_ptr);
  }

  if ((Size > 0 ? Data[0] : 0) & 2) {
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  }

  if ((Size > 0 ? Data[0] : 0) & 4) {
    png_set_rgb_to_gray_fixed(png_ptr, error_action, red, green);
    (void)png_get_rgb_to_gray_status(png_ptr);
  }

  if ((Size > 0 ? Data[0] : 0) & 8) {
    png_set_background(png_ptr, &background, background_gamma_code, need_expand,
                       (double)ReadFixed(Data, Size, off) / 100000.0);
  }

  if ((Size > 0 ? Data[0] : 0) & 16) {
    png_set_gamma_fixed(png_ptr, screen_gamma, override_file_gamma);
  }

  png_start_read_image(png_ptr);

  if ((Size > 0 ? Data[0] : 0) & 32) {
    png_start_read_image(png_ptr);
  }

  if ((Size > 0 ? Data[0] : 0) & 64) {
    png_set_strip_16(png_ptr);
    png_set_expand_gray_1_2_4_to_8(png_ptr);
    png_set_rgb_to_gray_fixed(png_ptr, error_action ^ 1, green, red);
    png_set_background(png_ptr, &background,
                       background_gamma_code & 3, need_expand ^ 1,
                       (double)1.0);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}