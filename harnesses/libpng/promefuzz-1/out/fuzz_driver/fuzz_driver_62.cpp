// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
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
#include <cstring>
#include <png.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  if (Size >= 2) {
    memcpy(buf2, Data, 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_32 u32 = png_get_uint_32(buf4);
  png_int_32 i32 = png_get_int_32(buf4);
  png_uint_16 u16 = png_get_uint_16(buf2);

  png_byte out4a[4] = {0, 0, 0, 0};
  png_byte out4b[4] = {0, 0, 0, 0};
  png_byte out2[2] = {0, 0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, i32);
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  (void)png_get_uint_32(out4a);
  (void)png_get_int_32(out4a);
  (void)png_get_uint_32(out4b);
  (void)png_get_int_32(out4b);
  (void)png_get_uint_16(out2);

  if (Size >= 8) {
    png_byte alt4[4];
    memcpy(alt4, Data + Size - 4, 4);
    png_uint_32 alt_u32 = png_get_uint_32(alt4);
    png_int_32 alt_i32 = png_get_int_32(alt4);
    png_save_uint_32(out4a, alt_u32);
    png_save_int_32(out4b, alt_i32);
    (void)png_get_uint_32(out4a);
    (void)png_get_int_32(out4b);
  }

  if (Size >= 4) {
    png_uint_32 mixed_u32 =
        (static_cast<png_uint_32>(buf2[0]) << 24) |
        (static_cast<png_uint_32>(buf2[1]) << 16) |
        (static_cast<png_uint_32>(out2[0]) << 8) |
        static_cast<png_uint_32>(out2[1]);
    png_save_uint_32(out4a, mixed_u32);
    (void)png_get_uint_32(out4a);
    (void)png_get_int_32(out4a);
  }

  png_save_uint_32(out4a, 0U);
  png_save_uint_32(out4a, 0xFFFFFFFFU);
  png_save_int_32(out4b, 0);
  png_save_int_32(out4b, static_cast<png_int_32>(0x7FFFFFFF));
  png_save_int_32(out4b, static_cast<png_int_32>(0x80000000u));
  png_save_uint_16(out2, 0U);
  png_save_uint_16(out2, 0xFFFFU);

  (void)png_get_uint_32(out4a);
  (void)png_get_int_32(out4b);
  (void)png_get_uint_16(out2);

  return 0;
}