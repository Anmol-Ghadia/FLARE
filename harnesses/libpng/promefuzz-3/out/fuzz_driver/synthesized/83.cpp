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
#include <setjmp.h>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_83(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size > 0) local4[0] = Data[0];
  if (Size > 1) local4[1] = Data[1];
  if (Size > 2) local4[2] = Data[2];
  if (Size > 3) local4[3] = Data[3];
  if (Size > 4) local2[0] = Data[4];
  else if (Size > 0) local2[0] = Data[0];
  if (Size > 5) local2[1] = Data[5];
  else if (Size > 1) local2[1] = Data[1];

  png_uint_32 u32 = png_get_uint_32(local4);
  png_uint_16 u16 = png_get_uint_16(local2);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out2[2] = {0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, static_cast<png_int_32>(u32));
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  if (Size >= 8) {
    png_uint_32 alt32 =
        (static_cast<png_uint_32>(Data[4]) << 24) |
        (static_cast<png_uint_32>(Data[5]) << 16) |
        (static_cast<png_uint_32>(Data[6]) << 8) |
        static_cast<png_uint_32>(Data[7]);
    png_save_uint_32(out4a, alt32);
    png_save_int_32(out4b, static_cast<png_int_32>(alt32 ^ u32));
  }

  if (Size >= 10) {
    unsigned int alt16 =
        (static_cast<unsigned int>(Data[8]) << 8) |
        static_cast<unsigned int>(Data[9]);
    png_save_uint_16(out2, alt16);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, local4);
      (void)png_get_uint_31(png_ptr, out4a);
      (void)png_get_uint_31(png_ptr, out4b);
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  png_uint_32 roundtrip1 = png_get_uint_32(out4a);
  png_uint_32 roundtrip2 = png_get_uint_32(out4b);
  png_uint_16 roundtrip3 = png_get_uint_16(out2);

  uint8_t mixed4[4] = {
      static_cast<uint8_t>(roundtrip1 >> 24),
      static_cast<uint8_t>(roundtrip2 >> 16),
      static_cast<uint8_t>(roundtrip1 >> 8),
      static_cast<uint8_t>(roundtrip2)
  };
  uint8_t mixed2[2] = {
      static_cast<uint8_t>(roundtrip3 >> 8),
      static_cast<uint8_t>(roundtrip3)
  };

  (void)png_get_uint_32(mixed4);
  (void)png_get_uint_16(mixed2);

  return 0;
}