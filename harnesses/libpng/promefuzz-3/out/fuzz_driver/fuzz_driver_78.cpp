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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size > 0) local4[0] = Data[0];
  if (Size > 1) local4[1] = Data[1];
  if (Size > 2) local4[2] = Data[2];
  if (Size > 3) local4[3] = Data[3];
  if (Size > 0) local2[0] = Data[0];
  if (Size > 1) local2[1] = Data[1];

  png_uint_32 u32 = png_get_uint_32(local4);
  png_uint_16 u16 = png_get_uint_16(local2);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out2[2] = {0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, static_cast<png_int_32>(u32));
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  png_uint_32 roundtrip_u32 = png_get_uint_32(out4a);
  png_uint_16 roundtrip_u16 = png_get_uint_16(out2);

  if (Size >= 8) {
    png_uint_32 alt_u32 =
        (static_cast<png_uint_32>(Data[4]) << 24) |
        (static_cast<png_uint_32>(Data[5]) << 16) |
        (static_cast<png_uint_32>(Data[6]) << 8) |
        static_cast<png_uint_32>(Data[7]);
    png_save_uint_32(out4a, alt_u32);
    (void)png_get_uint_32(out4a);
    png_save_int_32(out4b, static_cast<png_int_32>(alt_u32));
    (void)png_get_uint_32(out4b);
  }

  if (Size >= 6) {
    unsigned int alt_u16 =
        (static_cast<unsigned int>(Data[4]) << 8) |
        static_cast<unsigned int>(Data[5]);
    png_save_uint_16(out2, alt_u16);
    (void)png_get_uint_16(out2);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, local4);
      (void)png_get_uint_31(png_ptr, out4a);

      uint8_t max31[4] = {0x7f, 0xff, 0xff, 0xff};
      (void)png_get_uint_31(png_ptr, max31);

      uint8_t maybe_over[4] = {0x80, 0x00, 0x00, 0x00};
      if (Size > 0) {
        maybe_over[0] = Data[0];
        if ((Data[0] & 1) == 0) {
          maybe_over[0] |= 0x80;
        }
      }
      if (Size > 1) maybe_over[1] = Data[1];
      if (Size > 2) maybe_over[2] = Data[2];
      if (Size > 3) maybe_over[3] = Data[3];
      (void)png_get_uint_31(png_ptr, maybe_over);
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  volatile png_uint_32 sink32 = u32 ^ roundtrip_u32;
  volatile png_uint_16 sink16 = static_cast<png_uint_16>(u16 ^ roundtrip_u16);
  (void)sink32;
  (void)sink16;

  return 0;
}