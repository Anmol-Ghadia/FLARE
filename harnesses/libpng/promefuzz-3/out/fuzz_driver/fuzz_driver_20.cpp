// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
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
#include <stdio.h>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_bgr(nullptr);
  png_set_invert_alpha(nullptr);

  if (Size > 0) {
    if (Data[0] & 1) png_set_bgr(png_ptr);
    if (Data[0] & 2) png_set_strip_16(png_ptr);
    if (Data[0] & 4) png_set_scale_16(png_ptr);
    if (Data[0] & 8) png_set_invert_alpha(png_ptr);
  } else {
    png_set_bgr(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_scale_16(png_ptr);
    png_set_invert_alpha(png_ptr);
  }

  png_uint_32 skip1 = png_process_data_skip(png_ptr);
  (void)skip1;

  png_start_read_image(png_ptr);

  if (Size > 1) {
    if (Data[1] & 1) png_start_read_image(png_ptr);
    if (Data[1] & 2) {
      png_uint_32 skip2 = png_process_data_skip(png_ptr);
      (void)skip2;
    }
    if (Data[1] & 4) png_set_bgr(png_ptr);
    if (Data[1] & 8) png_set_invert_alpha(png_ptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}