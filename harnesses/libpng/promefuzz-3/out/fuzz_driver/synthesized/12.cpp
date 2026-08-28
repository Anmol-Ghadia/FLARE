// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_16 at pngwutil.c:37:1 in png.h
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
#include <cstdio>
#include <cstring>
#include <setjmp.h>

#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_12(const uint8_t *Data, size_t Size) {
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

  png_uint_32 u32 = png_get_uint_32(local4);
  png_uint_16 u16 = png_get_uint_16(local2);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out2[2] = {0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, static_cast<png_int_32>(u32));
  png_save_uint_16(out2, static_cast<unsigned int>(u16));

  if (Size >= 8) {
    png_uint_32 alt_u32 =
        (static_cast<png_uint_32>(Data[4]) << 24) |
        (static_cast<png_uint_32>(Data[5]) << 16) |
        (static_cast<png_uint_32>(Data[6]) << 8) |
        static_cast<png_uint_32>(Data[7]);
    png_save_uint_32(out4a, alt_u32);
    png_save_int_32(out4b, static_cast<png_int_32>(alt_u32 ^ u32));
  }

  if (Size >= 6) {
    unsigned int alt_u16 =
        (static_cast<unsigned int>(Data[4]) << 8) |
        static_cast<unsigned int>(Data[5]);
    png_save_uint_16(out2, alt_u16);
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    } else {
      fwrite(local4, 1, sizeof(local4), fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, local4);

      uint8_t safe31[4];
      memcpy(safe31, local4, 4);
      safe31[0] &= 0x7f;
      (void)png_get_uint_31(png_ptr, safe31);

      (void)png_get_uint_31(png_ptr, out4a);
      (void)png_get_uint_31(png_ptr, out4b);
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  png_uint_32 roundtrip_u32 = png_get_uint_32(out4a);
  png_uint_16 roundtrip_u16 = png_get_uint_16(out2);
  png_save_uint_32(out4a, roundtrip_u32 ^ static_cast<png_uint_32>(roundtrip_u16));
  png_save_int_32(out4b, static_cast<png_int_32>(roundtrip_u32));
  png_save_uint_16(out2, static_cast<unsigned int>(roundtrip_u16));

  return 0;
}