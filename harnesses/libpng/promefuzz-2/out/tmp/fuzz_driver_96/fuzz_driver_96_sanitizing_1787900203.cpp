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
#include <setjmp.h>
#include <vector>
#include <string.h>

static void FuzzBasicParsers(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  for (size_t i = 0; i < 4 && i < Size; ++i) local4[i] = Data[i];
  for (size_t i = 0; i < 2 && i < Size; ++i) local2[i] = Data[i];

  (void)png_get_uint_16(local2);
  (void)png_get_uint_32(local4);
  (void)png_get_int_32(local4);

  unsigned int v16 = 0;
  if (Size >= 2) {
    v16 = (static_cast<unsigned int>(Data[0]) << 8) |
          static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    v16 = static_cast<unsigned int>(Data[0]);
  }

  uint8_t out2[2] = {0, 0};
  png_save_uint_16(out2, v16);
  (void)png_get_uint_16(out2);

  if (Size >= 4) {
    for (size_t off = 0; off + 4 <= Size && off < 16; ++off) {
      (void)png_get_uint_32(Data + off);
      (void)png_get_int_32(Data + off);
    }
  }

  if (Size >= 2) {
    for (size_t off = 0; off + 2 <= Size && off < 16; ++off) {
      (void)png_get_uint_16(Data + off);
      uint8_t tmp[2];
      png_save_uint_16(tmp, png_get_uint_16(Data + off));
      (void)png_get_uint_16(tmp);
    }
  }
}

static void FuzzWithPngStructs(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  (void)png_get_signature(png_ptr, info_ptr);
  (void)png_get_signature(nullptr, info_ptr);
  (void)png_get_signature(png_ptr, nullptr);
  (void)png_get_signature(nullptr, nullptr);

  uint8_t local4[4] = {0, 0, 0, 0};
  for (size_t i = 0; i < 4 && i < Size; ++i) local4[i] = Data[i];

  (void)png_get_uint_31(png_ptr, local4);

  if (Size >= 4) {
    for (size_t off = 0; off + 4 <= Size && off < 16; ++off) {
      (void)png_get_uint_31(png_ptr, Data + off);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzBasicParsers(Data, Size);
  FuzzWithPngStructs(Data, Size);
  return 0;
}