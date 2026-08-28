// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <vector>
#include <cstdio>
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_74(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size > 0) local4[0] = Data[0];
  if (Size > 1) local4[1] = Data[1];
  if (Size > 2) local4[2] = Data[2];
  if (Size > 3) local4[3] = Data[3];

  if (Size > 0) local2[0] = Data[0];
  if (Size > 1) local2[1] = Data[1];

  png_uint_32 u32 = png_get_uint_32(local4);
  png_int_32 i32 = png_get_int_32(local4);
  png_uint_16 u16 = png_get_uint_16(local2);

  uint8_t out2[2] = {0, 0};
  png_save_uint_16(out2, static_cast<unsigned int>(u16));
  png_get_uint_16(out2);

  uint8_t derived4[4];
  derived4[0] = out2[0];
  derived4[1] = out2[1];
  derived4[2] = static_cast<uint8_t>(u32 & 0xff);
  derived4[3] = static_cast<uint8_t>(i32 & 0xff);

  png_get_uint_32(derived4);
  png_get_int_32(derived4);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      png_uint_32 safe31 = png_get_uint_31(png_ptr, local4);
      uint8_t out_from_31[2] = {0, 0};
      png_save_uint_16(out_from_31, static_cast<unsigned int>(safe31 & 0xffffu));
      png_get_uint_16(out_from_31);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_const_bytep sig = png_get_signature(png_ptr, info_ptr);
      if (sig != nullptr) {
        png_get_uint_32(sig);
        png_get_int_32(sig);
        png_get_uint_16(sig);
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    } else {
      png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    }
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  return 0;
}