// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
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
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <setjmp.h>
#include <png.h>

static png_uint_32 ReadBE32(const uint8_t* p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         static_cast<png_uint_32>(p[3]);
}

static unsigned int ReadBE16(const uint8_t* p) {
  return (static_cast<unsigned int>(p[0]) << 8) |
         static_cast<unsigned int>(p[1]);
}

extern "C" int LLVMFuzzerTestOneInput_58(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(local4, Data, 4);
  } else if (Size > 0) {
    memcpy(local4, Data, Size);
  }

  if (Size >= 2) {
    memcpy(local2, Data, 2);
  } else if (Size == 1) {
    local2[0] = Data[0];
  }

  png_uint_32 v32 = png_get_uint_32(local4);
  png_uint_16 v16 = png_get_uint_16(local2);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out2[2] = {0};

  png_save_uint_32(out4a, v32);
  png_save_int_32(out4b, static_cast<png_int_32>(v32));
  png_save_uint_16(out2, static_cast<unsigned int>(v16));

  (void)png_get_uint_32(out4a);
  (void)png_get_uint_32(out4b);
  (void)png_get_uint_16(out2);

  if (Size >= 8) {
    png_uint_32 alt32 = ReadBE32(Data + 4);
    png_save_uint_32(out4a, alt32);
    (void)png_get_uint_32(out4a);
    png_save_int_32(out4b, static_cast<png_int_32>(alt32));
    (void)png_get_uint_32(out4b);
  }

  if (Size >= 6) {
    unsigned int alt16 = ReadBE16(Data + Size - 2);
    png_save_uint_16(out2, alt16);
    (void)png_get_uint_16(out2);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, local4);

      uint8_t bounded[4];
      png_uint_32 bounded_val = v32 & 0x7fffffffU;
      png_save_uint_32(bounded, bounded_val);
      (void)png_get_uint_31(png_ptr, bounded);

      uint8_t max31[4] = {0x7f, 0xff, 0xff, 0xff};
      (void)png_get_uint_31(png_ptr, max31);

      uint8_t over31[4] = {0x80, 0x00, 0x00, 0x00};
      (void)png_get_uint_31(png_ptr, over31);
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  return 0;
}