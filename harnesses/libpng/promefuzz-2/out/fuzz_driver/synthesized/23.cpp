// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_gAMA at pngset.c:200:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_set_sCAL_s at pngset.c:430:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <vector>
#include <limits>
#include <cmath>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               static_cast<uint32_t>(data[off + 3]);
  off += 4;
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int32_t>(ReadU32(data, size, off));
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t &off) {
  uint64_t bits = 0;
  for (int i = 0; i < 8; ++i) {
    bits <<= 8;
    if (off < size) bits |= data[off++];
  }
  union {
    uint64_t u;
    double d;
  } conv;
  conv.u = bits;
  return conv.d;
}

static const char *MakeCStringFromBytes(const uint8_t *data, size_t len) {
  char *s = static_cast<char*>(malloc(len + 1));
  if (!s) return nullptr;
  if (len > 0) memcpy(s, data, len);
  s[len] = '\0';
  return s;
}

extern "C" int LLVMFuzzerTestOneInput_23(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  size_t off = 0;

  int unit = static_cast<int>(ReadU32(Data, Size, off) % 4);
  png_uint_32 res_x = ReadU32(Data, Size, off);
  png_uint_32 res_y = ReadU32(Data, Size, off);
  int unit_type = static_cast<int>(ReadU32(Data, Size, off));
  int srgb_intent = static_cast<int>(ReadU32(Data, Size, off) % 8);

  double gamma = ReadDoubleLike(Data, Size, off);
  double scal_w = ReadDoubleLike(Data, Size, off);
  double scal_h = ReadDoubleLike(Data, Size, off);

  png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_type);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);

  png_set_gAMA(png_ptr, info_ptr, gamma);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);

  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_cHRM);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);

  png_set_sCAL(png_ptr, info_ptr, unit, scal_w, scal_h);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);

  int out_unit = 0;
  double out_w = 0.0, out_h = 0.0;
  png_get_sCAL(png_ptr, info_ptr, &out_unit, &out_w, &out_h);

  if (off < Size) {
    size_t rem = Size - off;
    size_t len1 = rem / 2;
    size_t len2 = rem - len1;

    const char *swidth = MakeCStringFromBytes(Data + off, len1);
    const char *sheight = MakeCStringFromBytes(Data + off + len1, len2);

    if (swidth && sheight) {
      png_set_sCAL_s(png_ptr, info_ptr, unit, swidth, sheight);
      png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);

      int out_unit2 = 0;
      double out_w2 = 0.0, out_h2 = 0.0;
      png_get_sCAL(png_ptr, info_ptr, &out_unit2, &out_w2, &out_h2);
    }

    free(const_cast<char*>(swidth));
    free(const_cast<char*>(sheight));
  }

  for (int i = 0; i < 3; ++i) {
    png_uint_32 flag = 0;
    switch (i) {
      case 0: flag = PNG_INFO_sCAL; break;
      case 1: flag = PNG_INFO_gAMA; break;
      case 2: flag = PNG_INFO_pHYs; break;
    }
    png_get_valid(png_ptr, info_ptr, flag);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}