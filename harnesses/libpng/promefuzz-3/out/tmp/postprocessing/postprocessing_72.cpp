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

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t &off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static png_int_32 ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<png_int_32>(ReadU32(data, size, off));
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t &off) {
  png_int_32 s = ReadS32(data, size, off);
  return static_cast<double>(s) / 100000.0;
}

static void UserErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void UserWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, UserErrorFn, UserWarnFn);
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

  size_t off = 0;

  png_fixed_point wx = ReadS32(Data, Size, off);
  png_fixed_point wy = ReadS32(Data, Size, off);
  png_fixed_point rx = ReadS32(Data, Size, off);
  png_fixed_point ry = ReadS32(Data, Size, off);
  png_fixed_point gx = ReadS32(Data, Size, off);
  png_fixed_point gy = ReadS32(Data, Size, off);
  png_fixed_point bx = ReadS32(Data, Size, off);
  png_fixed_point by = ReadS32(Data, Size, off);

  double dwx = ReadDoubleLike(Data, Size, off);
  double dwy = ReadDoubleLike(Data, Size, off);
  double drx = ReadDoubleLike(Data, Size, off);
  double dry = ReadDoubleLike(Data, Size, off);
  double dgx = ReadDoubleLike(Data, Size, off);
  double dgy = ReadDoubleLike(Data, Size, off);
  double dbx = ReadDoubleLike(Data, Size, off);
  double dby = ReadDoubleLike(Data, Size, off);

  double gamma = ReadDoubleLike(Data, Size, off);
  png_fixed_point gamma_fixed = ReadS32(Data, Size, off);

#ifdef PNG_cHRM_SUPPORTED
  png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
  png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);

  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        png_set_cHRM_fixed(png_ptr, info_ptr, 31270, 32900, 64000, 33000, 30000,
                           60000, 15000, 6000);
        break;
      case 1:
        png_set_cHRM(png_ptr, info_ptr, 0.3127, 0.3290, 0.64, 0.33, 0.30, 0.60,
                     0.15, 0.06);
        break;
      case 2:
        png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
        png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);
        break;
      default:
        png_set_cHRM(png_ptr, info_ptr, -dwx, -dwy, -drx, -dry, -dgx, -dgy, -dbx,
                     -dby);
        break;
    }
  }

  double out_wx = 0.0, out_wy = 0.0, out_rx = 0.0, out_ry = 0.0;
  double out_gx = 0.0, out_gy = 0.0, out_bx = 0.0, out_by = 0.0;
  (void)png_get_cHRM(png_ptr, info_ptr, &out_wx, &out_wy, &out_rx, &out_ry,
                     &out_gx, &out_gy, &out_bx, &out_by);
  (void)png_get_cHRM(png_ptr, info_ptr, nullptr, &out_wy, nullptr, &out_ry,
                     nullptr, &out_gy, nullptr, &out_by);

  png_fixed_point f_wx = 0, f_wy = 0, f_rx = 0, f_ry = 0;
  png_fixed_point f_gx = 0, f_gy = 0, f_bx = 0, f_by = 0;
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, &f_wx, &f_wy, &f_rx, &f_ry, &f_gx,
                           &f_gy, &f_bx, &f_by);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, nullptr, &f_wy, nullptr, &f_ry,
                           nullptr, &f_gy, nullptr, &f_by);

  double red_X = 0.0, red_Y = 0.0, red_Z = 0.0;
  double green_X = 0.0, green_Y = 0.0, green_Z = 0.0;
  double blue_X = 0.0, blue_Y = 0.0, blue_Z = 0.0;
  (void)png_get_cHRM_XYZ(png_ptr, info_ptr, &red_X, &red_Y, &red_Z, &green_X,
                         &green_Y, &green_Z, &blue_X, &blue_Y, &blue_Z);
  (void)png_get_cHRM_XYZ(png_ptr, info_ptr, nullptr, &red_Y, nullptr, &green_X,
                         nullptr, &green_Z, nullptr, &blue_Y, nullptr);
#endif

#ifdef PNG_gAMA_SUPPORTED
  png_set_gAMA(png_ptr, info_ptr, gamma);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma_fixed);

  double out_gamma = 0.0;
  (void)png_get_gAMA(png_ptr, info_ptr, &out_gamma);
#endif

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}