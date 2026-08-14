// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ at pngset.c:118:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_XYZ at pngget.c:558:1 in png.h
// png_get_cHRM_XYZ at pngget.c:558:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_eXIf at pngget.c:779:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <cstdio>
#include <cstdlib>
#include <cstring>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 <= size) {
    uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
                 (static_cast<uint32_t>(data[off + 1]) << 16) |
                 (static_cast<uint32_t>(data[off + 2]) << 8) |
                 static_cast<uint32_t>(data[off + 3]);
    off += 4;
    return v;
  }
  uint32_t v = 0;
  while (off < size) {
    v = (v << 8) | data[off++];
  }
  return v;
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t &off) {
  uint32_t a = ReadU32(data, size, off);
  int32_t s = static_cast<int32_t>(a);
  return static_cast<double>(s) / 100000.0;
}

static png_fixed_point ReadFixed(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<png_fixed_point>(static_cast<png_int_32>(ReadU32(data, size, off)));
}

static void PngErrorFn(png_structp, png_const_charp) {
  std::abort();
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_65(const uint8_t *Data, size_t Size) {
  {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      if (Size > 0) std::fwrite(Data, 1, Size, fp);
      std::fclose(fp);
    }
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

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

  png_fixed_point f_white_x = ReadFixed(Data, Size, off);
  png_fixed_point f_white_y = ReadFixed(Data, Size, off);
  png_fixed_point f_red_x = ReadFixed(Data, Size, off);
  png_fixed_point f_red_y = ReadFixed(Data, Size, off);
  png_fixed_point f_green_x = ReadFixed(Data, Size, off);
  png_fixed_point f_green_y = ReadFixed(Data, Size, off);
  png_fixed_point f_blue_x = ReadFixed(Data, Size, off);
  png_fixed_point f_blue_y = ReadFixed(Data, Size, off);

  png_fixed_point f_red_X = ReadFixed(Data, Size, off);
  png_fixed_point f_red_Y = ReadFixed(Data, Size, off);
  png_fixed_point f_red_Z = ReadFixed(Data, Size, off);
  png_fixed_point f_green_X = ReadFixed(Data, Size, off);
  png_fixed_point f_green_Y = ReadFixed(Data, Size, off);
  png_fixed_point f_green_Z = ReadFixed(Data, Size, off);
  png_fixed_point f_blue_X = ReadFixed(Data, Size, off);
  png_fixed_point f_blue_Y = ReadFixed(Data, Size, off);
  png_fixed_point f_blue_Z = ReadFixed(Data, Size, off);

#ifdef PNG_cHRM_SUPPORTED
  png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y,
               green_x, green_y, blue_x, blue_y);
  png_set_cHRM_XYZ(png_ptr, info_ptr, red_X, red_Y, red_Z,
                   green_X, green_Y, green_Z, blue_X, blue_Y, blue_Z);
  png_set_cHRM_fixed(png_ptr, info_ptr, f_white_x, f_white_y, f_red_x, f_red_y,
                     f_green_x, f_green_y, f_blue_x, f_blue_y);
  png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, f_red_X, f_red_Y, f_red_Z,
                         f_green_X, f_green_Y, f_green_Z,
                         f_blue_X, f_blue_Y, f_blue_Z);
#endif

  double out_white_x = 0.0, out_white_y = 0.0, out_red_x = 0.0, out_red_y = 0.0;
  double out_green_x = 0.0, out_green_y = 0.0, out_blue_x = 0.0, out_blue_y = 0.0;
  double out_red_X = 0.0, out_red_Y = 0.0, out_red_Z = 0.0;
  double out_green_X = 0.0, out_green_Y = 0.0, out_green_Z = 0.0;
  double out_blue_X = 0.0, out_blue_Y = 0.0, out_blue_Z = 0.0;

  png_fixed_point fout_white_x = 0, fout_white_y = 0, fout_red_x = 0, fout_red_y = 0;
  png_fixed_point fout_green_x = 0, fout_green_y = 0, fout_blue_x = 0, fout_blue_y = 0;
  png_fixed_point fout_red_X = 0, fout_red_Y = 0, fout_red_Z = 0;
  png_fixed_point fout_green_X = 0, fout_green_Y = 0, fout_green_Z = 0;
  png_fixed_point fout_blue_X = 0, fout_blue_Y = 0, fout_blue_Z = 0;

  (void)png_get_cHRM(png_ptr, info_ptr,
                     &out_white_x, &out_white_y, &out_red_x, &out_red_y,
                     &out_green_x, &out_green_y, &out_blue_x, &out_blue_y);
  (void)png_get_cHRM(png_ptr, info_ptr,
                     nullptr, nullptr, &out_red_x, nullptr,
                     nullptr, &out_green_y, &out_blue_x, nullptr);

  (void)png_get_cHRM_fixed(png_ptr, info_ptr,
                           &fout_white_x, &fout_white_y, &fout_red_x, &fout_red_y,
                           &fout_green_x, &fout_green_y, &fout_blue_x, &fout_blue_y);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr,
                           nullptr, nullptr, &fout_red_x, nullptr,
                           nullptr, &fout_green_y, &fout_blue_x, nullptr);

  (void)png_get_cHRM_XYZ(png_ptr, info_ptr,
                         &out_red_X, &out_red_Y, &out_red_Z,
                         &out_green_X, &out_green_Y, &out_green_Z,
                         &out_blue_X, &out_blue_Y, &out_blue_Z);
  (void)png_get_cHRM_XYZ(png_ptr, info_ptr,
                         &out_red_X, nullptr, &out_red_Z,
                         nullptr, &out_green_Y, nullptr,
                         &out_blue_X, nullptr, &out_blue_Z);

  (void)png_get_cHRM_XYZ_fixed(png_ptr, info_ptr,
                               &fout_red_X, &fout_red_Y, &fout_red_Z,
                               &fout_green_X, &fout_green_Y, &fout_green_Z,
                               &fout_blue_X, &fout_blue_Y, &fout_blue_Z);
  (void)png_get_cHRM_XYZ_fixed(png_ptr, info_ptr,
                               &fout_red_X, nullptr, &fout_red_Z,
                               nullptr, &fout_green_Y, nullptr,
                               &fout_blue_X, nullptr, &fout_blue_Z);

#ifdef PNG_eXIf_SUPPORTED
  png_bytep exif = nullptr;
  (void)png_get_eXIf(png_ptr, info_ptr, &exif);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}