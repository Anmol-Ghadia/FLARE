// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ at pngset.c:118:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ at pngset.c:118:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
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
#include <setjmp.h>
#include <vector>
#include <limits>

static uint32_t ReadU32(const uint8_t *Data, size_t Size, size_t &Offset) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (Offset < Size) v |= Data[Offset++];
  }
  return v;
}

static int32_t ReadS32(const uint8_t *Data, size_t Size, size_t &Offset) {
  return static_cast<int32_t>(ReadU32(Data, Size, Offset));
}

static double ReadDoubleLike(const uint8_t *Data, size_t Size, size_t &Offset) {
  uint64_t bits = 0;
  for (int i = 0; i < 8; ++i) {
    bits <<= 8;
    if (Offset < Size) bits |= Data[Offset++];
  }

  union {
    uint64_t u;
    double d;
  } conv;
  conv.u = bits;
  return conv.d;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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

  png_set_IHDR(png_ptr, info_ptr,
               1, 1, 8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  size_t off = 0;

  double white_x = ReadDoubleLike(Data, Size, off);
  double white_y = ReadDoubleLike(Data, Size, off);
  double red_x = ReadDoubleLike(Data, Size, off);
  double red_y = ReadDoubleLike(Data, Size, off);
  double green_x = ReadDoubleLike(Data, Size, off);
  double green_y = ReadDoubleLike(Data, Size, off);
  double blue_x = ReadDoubleLike(Data, Size, off);
  double blue_y = ReadDoubleLike(Data, Size, off);

  double red_X = ReadDoubleLike(Data, Size, off);
  double red_Y = ReadDoubleLike(Data, Size, off);
  double red_Z = ReadDoubleLike(Data, Size, off);
  double green_X = ReadDoubleLike(Data, Size, off);
  double green_Y = ReadDoubleLike(Data, Size, off);
  double green_Z = ReadDoubleLike(Data, Size, off);
  double blue_X = ReadDoubleLike(Data, Size, off);
  double blue_Y = ReadDoubleLike(Data, Size, off);
  double blue_Z = ReadDoubleLike(Data, Size, off);

  double gamma = ReadDoubleLike(Data, Size, off);
  int srgb_intent = static_cast<int>(ReadS32(Data, Size, off));

  png_fixed_point fx[17];
  for (int i = 0; i < 17; ++i) {
    fx[i] = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
  }

  png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y);
  png_set_cHRM_XYZ(png_ptr, info_ptr, red_X, red_Y, red_Z, green_X, green_Y, green_Z, blue_X, blue_Y, blue_Z);
  png_set_gAMA(png_ptr, info_ptr, gamma);
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

  png_set_cHRM_fixed(png_ptr, info_ptr,
                     fx[0], fx[1], fx[2], fx[3], fx[4], fx[5], fx[6], fx[7]);

  png_set_cHRM_XYZ_fixed(png_ptr, info_ptr,
                         fx[8], fx[9], fx[10], fx[11], fx[12], fx[13], fx[14], fx[15], fx[16]);

  png_set_gAMA_fixed(png_ptr, info_ptr, static_cast<png_fixed_point>(ReadS32(Data, Size, off)));

  double out_white_x = 0.0, out_white_y = 0.0, out_red_x = 0.0, out_red_y = 0.0;
  double out_green_x = 0.0, out_green_y = 0.0, out_blue_x = 0.0, out_blue_y = 0.0;
  png_get_cHRM(png_ptr, info_ptr,
               &out_white_x, &out_white_y, &out_red_x, &out_red_y,
               &out_green_x, &out_green_y, &out_blue_x, &out_blue_y);

  png_get_cHRM(png_ptr, info_ptr,
               nullptr, &out_white_y, nullptr, &out_red_y,
               &out_green_x, nullptr, &out_blue_x, nullptr);

  png_fixed_point out_red_X = 0, out_red_Y = 0, out_red_Z = 0;
  png_fixed_point out_green_X = 0, out_green_Y = 0, out_green_Z = 0;
  png_fixed_point out_blue_X = 0, out_blue_Y = 0, out_blue_Z = 0;

  png_get_cHRM_XYZ_fixed(png_ptr, info_ptr,
                         &out_red_X, &out_red_Y, &out_red_Z,
                         &out_green_X, &out_green_Y, &out_green_Z,
                         &out_blue_X, &out_blue_Y, &out_blue_Z);

  png_get_cHRM_XYZ_fixed(png_ptr, info_ptr,
                         nullptr, &out_red_Y, nullptr,
                         &out_green_X, nullptr, &out_green_Z,
                         nullptr, &out_blue_Y, nullptr);

  png_get_cHRM(png_ptr, nullptr,
               &out_white_x, &out_white_y, &out_red_x, &out_red_y,
               &out_green_x, &out_green_y, &out_blue_x, &out_blue_y);

  png_get_cHRM_XYZ_fixed(png_ptr, nullptr,
                         &out_red_X, &out_red_Y, &out_red_Z,
                         &out_green_X, &out_green_Y, &out_green_Z,
                         &out_blue_X, &out_blue_Y, &out_blue_Z);

  png_set_cHRM(png_ptr, nullptr, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y);
  png_set_cHRM_XYZ(png_ptr, nullptr, red_X, red_Y, red_Z, green_X, green_Y, green_Z, blue_X, blue_Y, blue_Z);
  png_set_gAMA(png_ptr, nullptr, gamma);
  png_set_sRGB_gAMA_and_cHRM(png_ptr, nullptr, srgb_intent);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}