// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <png.h>

static png_uint_32 ReadU32BE(const uint8_t* p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         static_cast<png_uint_32>(p[3]);
}

static png_uint_16 ReadU16BE(const uint8_t* p) {
  return static_cast<png_uint_16>(
      (static_cast<unsigned int>(p[0]) << 8) |
      static_cast<unsigned int>(p[1]));
}

extern "C" int LLVMFuzzerTestOneInput_86(const uint8_t *Data, size_t Size) {
  uint8_t in4[4] = {0, 0, 0, 0};
  uint8_t in2[2] = {0, 0};

  for (size_t i = 0; i < 4 && i < Size; ++i) in4[i] = Data[i];
  for (size_t i = 0; i < 2 && i < Size; ++i) in2[i] = Data[i];

  png_uint_32 u32 = png_get_uint_32(reinterpret_cast<png_const_bytep>(in4));
  png_int_32 i32 = png_get_int_32(reinterpret_cast<png_const_bytep>(in4));
  png_uint_16 u16 = png_get_uint_16(reinterpret_cast<png_const_bytep>(in2));

  uint8_t out4a[4] = {0, 0, 0, 0};
  uint8_t out4b[4] = {0, 0, 0, 0};
  uint8_t out2[2] = {0, 0};

  png_save_uint_32(reinterpret_cast<png_bytep>(out4a), u32);
  png_save_int_32(reinterpret_cast<png_bytep>(out4b), i32);
  png_save_uint_16(reinterpret_cast<png_bytep>(out2), static_cast<unsigned int>(u16));

  (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(out4a));
  (void)png_get_int_32(reinterpret_cast<png_const_bytep>(out4a));
  (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(out4b));
  (void)png_get_int_32(reinterpret_cast<png_const_bytep>(out4b));
  (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(out2));

  if (Size >= 4) {
    png_uint_32 manual_u32 = ReadU32BE(Data);
    uint8_t tmp4[4];
    png_save_uint_32(reinterpret_cast<png_bytep>(tmp4), manual_u32);
    (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(tmp4));

    png_int_32 derived_i32 = static_cast<png_int_32>(manual_u32);
    png_save_int_32(reinterpret_cast<png_bytep>(tmp4), derived_i32);
    (void)png_get_int_32(reinterpret_cast<png_const_bytep>(tmp4));
  }

  if (Size >= 2) {
    png_uint_16 manual_u16 = ReadU16BE(Data);
    uint8_t tmp2[2];
    png_save_uint_16(reinterpret_cast<png_bytep>(tmp2), static_cast<unsigned int>(manual_u16));
    (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(tmp2));
  }

  if (Size > 0) {
    png_uint_32 accum32 = 0;
    for (size_t i = 0; i < Size; ++i) {
      accum32 = (accum32 * 33u) ^ Data[i];
    }

    uint8_t tmp4[4];
    uint8_t tmp2[2];

    png_save_uint_32(reinterpret_cast<png_bytep>(tmp4), accum32);
    (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(tmp4));
    (void)png_get_int_32(reinterpret_cast<png_const_bytep>(tmp4));

    png_save_int_32(reinterpret_cast<png_bytep>(tmp4), static_cast<png_int_32>(accum32));
    (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(tmp4));
    (void)png_get_int_32(reinterpret_cast<png_const_bytep>(tmp4));

    png_save_uint_16(reinterpret_cast<png_bytep>(tmp2), static_cast<unsigned int>(accum32));
    (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(tmp2));
  }

  if (Size >= 8) {
    for (size_t off = 0; off + 4 <= Size && off < 16; ++off) {
      png_uint_32 v = png_get_uint_32(reinterpret_cast<png_const_bytep>(Data + off));
      png_int_32 s = png_get_int_32(reinterpret_cast<png_const_bytep>(Data + off));
      uint8_t tmp4[4];
      png_save_uint_32(reinterpret_cast<png_bytep>(tmp4), v ^ static_cast<png_uint_32>(off));
      png_save_int_32(reinterpret_cast<png_bytep>(tmp4), s);
      (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(tmp4));
      (void)png_get_int_32(reinterpret_cast<png_const_bytep>(tmp4));
    }

    for (size_t off = 0; off + 2 <= Size && off < 16; ++off) {
      png_uint_16 v = png_get_uint_16(reinterpret_cast<png_const_bytep>(Data + off));
      uint8_t tmp2[2];
      png_save_uint_16(reinterpret_cast<png_bytep>(tmp2), static_cast<unsigned int>(v + off));
      (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(tmp2));
    }
  }

  return 0;
}