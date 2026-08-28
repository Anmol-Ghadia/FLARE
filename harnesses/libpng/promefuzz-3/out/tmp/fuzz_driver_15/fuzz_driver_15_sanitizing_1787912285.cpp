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
#include <setjmp.h>
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 4 > size) return 0;
  uint32_t v = ((uint32_t)data[*off] << 24) |
               ((uint32_t)data[*off + 1] << 16) |
               ((uint32_t)data[*off + 2] << 8) |
               (uint32_t)data[*off + 3];
  *off += 4;
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t *off) {
  return (int32_t)ReadU32(data, size, off);
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t *off) {
  uint64_t hi = ReadU32(data, size, off);
  uint64_t lo = ReadU32(data, size, off);
  uint64_t bits = (hi << 32) | lo;
  union {
    uint64_t u;
    double d;
  } conv;
  conv.u = bits;
  return conv.d;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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

  size_t off = 0;

  double white_x = ReadDoubleLike(Data, Size, &off);
  double white_y = ReadDoubleLike(Data, Size, &off);
  double red_x = ReadDoubleLike(Data, Size, &off);
  double red_y = ReadDoubleLike(Data, Size, &off);
  double green_x = ReadDoubleLike(Data, Size, &off);
  double green_y = ReadDoubleLike(Data, Size, &off);
  double blue_x = ReadDoubleLike(Data, Size, &off);
  double blue_y = ReadDoubleLike(Data, Size, &off);

  double red_X = ReadDoubleLike(Data, Size, &off);
  double red_Y = ReadDoubleLike(Data, Size, &off);
  double red_Z = ReadDoubleLike(Data, Size, &off);
  double green_X = ReadDoubleLike(Data, Size, &off);
  double green_Y = ReadDoubleLike(Data, Size, &off);
  double green_Z = ReadDoubleLike(Data, Size, &off);
  double blue_X = ReadDoubleLike(Data, Size, &off);
  double blue_Y = ReadDoubleLike(Data, Size, &off);
  double blue_Z = ReadDoubleLike(Data, Size, &off);

  png_fixed_point f_white_x = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_white_y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_red_x = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_red_y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_green_x = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_green_y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_blue_x = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_blue_y = (png_fixed_point)ReadS32(Data, Size, &off);

  png_fixed_point f_red_X = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_red_Y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_red_Z = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_green_X = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_green_Y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_green_Z = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_blue_X = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_blue_Y = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point f_blue_Z = (png_fixed_point)ReadS32(Data, Size, &off);

  double gamma_d = ReadDoubleLike(Data, Size, &off);
  png_fixed_point gamma_f = (png_fixed_point)ReadS32(Data, Size, &off);
  int srgb_intent = (int)(ReadU32(Data, Size, &off) % 8);

  png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y, green_x, green_y,
               blue_x, blue_y);
  png_set_cHRM_fixed(png_ptr, info_ptr, f_white_x, f_white_y, f_red_x, f_red_y,
                     f_green_x, f_green_y, f_blue_x, f_blue_y);
  png_set_cHRM_XYZ(png_ptr, info_ptr, red_X, red_Y, red_Z, green_X, green_Y, green_Z,
                   blue_X, blue_Y, blue_Z);
  png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, f_red_X, f_red_Y, f_red_Z, f_green_X,
                         f_green_Y, f_green_Z, f_blue_X, f_blue_Y, f_blue_Z);
  png_set_gAMA(png_ptr, info_ptr, gamma_d);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma_f);
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, PNG_sRGB_INTENT_PERCEPTUAL);
        break;
      case 1:
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, PNG_sRGB_INTENT_RELATIVE);
        break;
      case 2:
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, PNG_sRGB_INTENT_SATURATION);
        break;
      case 3:
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE);
        break;
    }
  }

  png_set_cHRM(png_ptr, info_ptr, blue_x, blue_y, green_x, green_y, red_x, red_y,
               white_x, white_y);
  png_set_cHRM_XYZ(png_ptr, info_ptr, blue_X, blue_Y, blue_Z, red_X, red_Y, red_Z,
                   green_X, green_Y, green_Z);
  png_set_gAMA(png_ptr, info_ptr, -gamma_d);
  png_set_gAMA_fixed(png_ptr, info_ptr, (png_fixed_point)(-gamma_f));

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}