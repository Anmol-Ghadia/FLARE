// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
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
#include <setjmp.h>
#include <vector>

#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local4b[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(local4, Data, 4);
  } else if (Size > 0) {
    memcpy(local4, Data, Size);
  }

  png_uint_32 u32 = png_get_uint_32(local4);
  png_int_32 i32 = png_get_int_32(local4);

  png_save_uint_32(local4b, u32);
  png_save_int_32(local4b, i32);

  unsigned int u16src = 0;
  if (Size >= 2) {
    u16src = (static_cast<unsigned int>(Data[0]) << 8) |
             static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    u16src = static_cast<unsigned int>(Data[0]);
  }
  png_save_uint_16(local2, u16src);

  png_save_uint_32(local4b, static_cast<png_uint_32>(i32));
  png_get_uint_32(local4b);
  png_get_int_32(local4b);

  if (Size >= 8) {
    png_save_uint_32(local4b, png_get_uint_32(Data + 4));
    png_get_uint_32(local4b);
    png_get_int_32(local4b);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      png_get_uint_31(png_ptr, local4);

      uint8_t max31[4] = {0x7f, 0xff, 0xff, 0xff};
      png_get_uint_31(png_ptr, max31);

      png_get_uint_31(png_ptr, local4b);

      if (Size >= 4) {
        png_get_uint_31(png_ptr, Data);
      }
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  return 0;
}