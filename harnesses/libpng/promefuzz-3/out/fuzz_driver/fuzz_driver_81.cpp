// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
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
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <png.h>
#include <setjmp.h>

static void SilentErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void SilentWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size > 0) {
    size_t copy4 = Size < 4 ? Size : 4;
    memcpy(local4, Data, copy4);
    size_t copy2 = Size < 2 ? Size : 2;
    memcpy(local2, Data, copy2);
  }

  png_uint_32 u32 = png_get_uint_32(local4);
  png_uint_16 u16 = png_get_uint_16(local2);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out2[2] = {0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, static_cast<png_int_32>(u32));
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  png_uint_32 mixed32 = u32;
  png_uint_16 mixed16 = u16;

  if (Size >= 8) {
    mixed32 ^= (static_cast<png_uint_32>(Data[4]) << 24) |
               (static_cast<png_uint_32>(Data[5]) << 16) |
               (static_cast<png_uint_32>(Data[6]) << 8) |
               static_cast<png_uint_32>(Data[7]);
  }
  if (Size >= 10) {
    mixed16 ^= static_cast<png_uint_16>(
        (static_cast<unsigned int>(Data[8]) << 8) | static_cast<unsigned int>(Data[9]));
  }

  png_save_uint_32(out4a, mixed32);
  png_save_int_32(out4b, static_cast<png_int_32>(mixed32 ^ 0x80000000u));
  png_save_uint_16(out2, mixed16);

  (void)png_get_uint_32(out4a);
  (void)png_get_uint_32(out4b);
  (void)png_get_uint_16(out2);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, SilentErrorFn, SilentWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, local4);
      (void)png_get_uint_31(png_ptr, out4a);
      (void)png_get_uint_31(png_ptr, out4b);
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  return 0;
}