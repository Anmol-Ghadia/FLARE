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
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t in4[4] = {0, 0, 0, 0};
  uint8_t in2[2] = {0, 0};
  uint8_t out4[4] = {0, 0, 0, 0};
  uint8_t out4b[4] = {0, 0, 0, 0};
  uint8_t out2[2] = {0, 0};

  if (Size > 0) in4[0] = Data[0];
  if (Size > 1) in4[1] = Data[1];
  if (Size > 2) in4[2] = Data[2];
  if (Size > 3) in4[3] = Data[3];

  if (Size > 4) in2[0] = Data[4];
  else if (Size > 0) in2[0] = Data[0];

  if (Size > 5) in2[1] = Data[5];
  else if (Size > 1) in2[1] = Data[1];

  png_uint_32 u32 = png_get_uint_32(in4);
  png_uint_16 u16 = png_get_uint_16(in2);

  png_save_uint_32(out4, u32);
  png_save_int_32(out4b, static_cast<png_int_32>(u32));
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  png_uint_32 derived32 = u32;
  if (Size >= 8) {
    derived32 ^= (static_cast<png_uint_32>(Data[4]) << 24) |
                 (static_cast<png_uint_32>(Data[5]) << 16) |
                 (static_cast<png_uint_32>(Data[6]) << 8) |
                 static_cast<png_uint_32>(Data[7]);
  } else {
    derived32 ^= static_cast<png_uint_32>(Size);
  }

  png_save_uint_32(out4, derived32);
  png_save_int_32(out4b, static_cast<png_int_32>(~derived32));
  png_save_uint_16(out2, static_cast<unsigned int>((u16 ^ static_cast<png_uint_16>(derived32)) & 0xFFFFu));

  (void)png_get_uint_32(out4);
  (void)png_get_uint_32(out4b);
  (void)png_get_uint_16(out2);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, in4);
    }

    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      uint8_t safe31[4];
      std::memcpy(safe31, out4, 4);
      safe31[0] &= 0x7F;
      (void)png_get_uint_31(png_ptr, safe31);
    }

    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      uint8_t max31[4] = {0x7F, 0xFF, 0xFF, 0xFF};
      if (Size > 0 && (Data[0] & 1)) {
        max31[0] = 0x80;
      }
      (void)png_get_uint_31(png_ptr, max31);
    }

    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  return 0;
}