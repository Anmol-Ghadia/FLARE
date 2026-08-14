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
#include <stdio.h>
#include <setjmp.h>
#include <vector>
#include <string>
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               (static_cast<uint32_t>(data[off + 3]));
  off += 4;
  return v;
}

static int ReadI32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
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
  if (conv.d != conv.d || conv.d == 1.0 / 0.0 || conv.d == -1.0 / 0.0)
    return 1.0;
  return conv.d;
}

static png_fixed_point ReadFixed(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<png_fixed_point>(ReadI32(data, size, off));
}

static std::string ReadString(const uint8_t *data, size_t size, size_t &off) {
  if (off >= size) return std::string();
  uint32_t len = ReadU32(data, size, off);
  size_t remaining = size - off;
  size_t take = std::min<size_t>(len % 128, remaining);
  std::string s(reinterpret_cast<const char*>(data + off), take);
  off += take;
  return s;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
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

  size_t off = 0;

  int unit1 = (ReadI32(Data, Size, off) % 5) - 1;
  int unit2 = (ReadI32(Data, Size, off) % 5) - 1;
  int unit3 = (ReadI32(Data, Size, off) % 5) - 1;

  double width_d1 = ReadDoubleLike(Data, Size, off);
  double height_d1 = ReadDoubleLike(Data, Size, off);
  double width_d2 = ReadDoubleLike(Data, Size, off);
  double height_d2 = ReadDoubleLike(Data, Size, off);

  png_fixed_point width_f1 = ReadFixed(Data, Size, off);
  png_fixed_point height_f1 = ReadFixed(Data, Size, off);
  png_fixed_point width_f2 = ReadFixed(Data, Size, off);
  png_fixed_point height_f2 = ReadFixed(Data, Size, off);

  std::string sw1 = ReadString(Data, Size, off);
  std::string sh1 = ReadString(Data, Size, off);
  std::string sw2 = ReadString(Data, Size, off);
  std::string sh2 = ReadString(Data, Size, off);

  if (sw1.empty()) sw1 = "1.0";
  if (sh1.empty()) sh1 = "1.0";
  if (sw2.empty()) sw2 = "0";
  if (sh2.empty()) sh2 = "0";

  png_set_sCAL_s(png_ptr, info_ptr, unit1, sw1.c_str(), sh1.c_str());

#if defined(PNG_FLOATING_POINT_SUPPORTED) || defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
  png_set_sCAL(png_ptr, info_ptr, unit2, width_d1, height_d1);
  png_set_sCAL(png_ptr, info_ptr, 1 + (unit2 & 1), width_d2, height_d2);
#endif

#ifdef PNG_FIXED_POINT_SUPPORTED
  png_set_sCAL_fixed(png_ptr, info_ptr, unit3, width_f1, height_f1);
  png_set_sCAL_fixed(png_ptr, info_ptr, 1 + (unit3 & 1), width_f2, height_f2);
#endif

  png_set_sCAL_s(png_ptr, info_ptr, 1 + (unit1 & 1), sw2.c_str(), sh2.c_str());

  int out_unit = 0;
  double out_w = 0.0, out_h = 0.0;
  png_fixed_point out_fw = 0, out_fh = 0;
  char *out_sw = nullptr;
  char *out_sh = nullptr;

#if defined(PNG_FLOATING_POINT_SUPPORTED) || defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
  (void)png_get_sCAL(png_ptr, info_ptr, &out_unit, &out_w, &out_h);
#endif

#if defined(PNG_FLOATING_POINT_SUPPORTED) || defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &out_unit, &out_fw, &out_fh);
#endif

  (void)png_get_sCAL_s(png_ptr, info_ptr, &out_unit, &out_sw, &out_sh);

  int alt_unit = 12345;
  double alt_w = -1.0, alt_h = -1.0;
  png_fixed_point alt_fw = -1, alt_fh = -1;
  char *alt_sw = nullptr;
  char *alt_sh = nullptr;

#if defined(PNG_FLOATING_POINT_SUPPORTED) || defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
  (void)png_get_sCAL(png_ptr, nullptr, &alt_unit, &alt_w, &alt_h);
#endif

#if defined(PNG_FLOATING_POINT_SUPPORTED) || defined(PNG_FLOATING_ARITHMETIC_SUPPORTED)
  (void)png_get_sCAL_fixed(png_ptr, nullptr, &alt_unit, &alt_fw, &alt_fh);
#endif

  (void)png_get_sCAL_s(png_ptr, nullptr, &alt_unit, &alt_sw, &alt_sh);
  (void)png_get_sCAL_s(nullptr, info_ptr, &alt_unit, &alt_sw, &alt_sh);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}