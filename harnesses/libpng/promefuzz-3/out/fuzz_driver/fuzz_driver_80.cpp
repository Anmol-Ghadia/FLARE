// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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

static void fuzz_basic_integer_apis(const uint8_t* Data, size_t Size) {
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else {
    for (size_t i = 0; i < Size; ++i) buf4[i] = Data[i];
  }

  if (Size >= 2) {
    memcpy(buf2, Data, 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_32 u32 = png_get_uint_32(buf4);
  png_int_32 i32 = png_get_int_32(buf4);
  png_uint_16 u16 = png_get_uint_16(buf2);

  png_byte out4[4];
  png_byte out2[2];
  png_save_uint_32(out4, u32);
  png_save_uint_32(out4, static_cast<png_uint_32>(i32));
  png_save_uint_16(out2, u16);

  if (Size >= 8) {
    png_byte alt4[4];
    memcpy(alt4, Data + (Size - 4), 4);
    png_uint_32 u32b = png_get_uint_32(alt4);
    png_int_32 i32b = png_get_int_32(alt4);
    png_save_uint_32(out4, u32b ^ static_cast<png_uint_32>(i32b));
  }

  if (Size >= 6) {
    png_byte alt2[2];
    memcpy(alt2, Data + (Size - 2), 2);
    png_uint_16 u16b = png_get_uint_16(alt2);
    png_save_uint_16(out2, static_cast<unsigned int>(u16b ^ u16));
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  fuzz_basic_integer_apis(Data, Size);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0) {
    png_byte buf4[4] = {0, 0, 0, 0};
    if (Size >= 4) {
      memcpy(buf4, Data, 4);
    } else {
      for (size_t i = 0; i < Size; ++i) buf4[i] = Data[i];
    }

    (void)png_get_uint_31(png_ptr, buf4);

    png_byte saved4[4];
    png_uint_32 values[4];
    values[0] = png_get_uint_32(buf4);
    values[1] = static_cast<png_uint_32>(png_get_int_32(buf4));
    values[2] = (Size > 0) ? static_cast<png_uint_32>(Data[0]) << 24 : 0;
    values[3] = 0x7fffffffU;

    for (int i = 0; i < 4; ++i) {
      png_save_uint_32(saved4, values[i]);
      if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        (void)png_get_uint_31(png_ptr, saved4);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}