// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>

static void png_noop_error(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void png_noop_warning(png_structp, png_const_charp) {
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return ((uint32_t)data[offset] << 24) |
         ((uint32_t)data[offset + 1] << 16) |
         ((uint32_t)data[offset + 2] << 8) |
         (uint32_t)data[offset + 3];
}

static int32_t read_s32(const uint8_t *data, size_t size, size_t offset) {
  return (int32_t)read_u32(data, size, offset);
}

static double make_double_from_bytes(const uint8_t *data, size_t size, size_t offset) {
  uint64_t bits = 0;
  for (size_t i = 0; i < 8; ++i) {
    bits <<= 8;
    if (offset + i < size) bits |= data[offset + i];
  }
  double d;
  memcpy(&d, &bits, sizeof(d));
  return d;
}

extern "C" int LLVMFuzzerTestOneInput_34(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_noop_error, png_noop_warning);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  double gamma1 = make_double_from_bytes(Data, Size, 0);
  double gamma2 = make_double_from_bytes(Data, Size, 8);
  double width_d = make_double_from_bytes(Data, Size, 16);
  double height_d = make_double_from_bytes(Data, Size, 24);

  png_fixed_point fixed_gamma1 = (png_fixed_point)read_s32(Data, Size, 32);
  png_fixed_point fixed_gamma2 = (png_fixed_point)read_s32(Data, Size, 36);
  png_fixed_point fixed_width = (png_fixed_point)read_s32(Data, Size, 40);
  png_fixed_point fixed_height = (png_fixed_point)read_s32(Data, Size, 44);

  int srgb_intent1 = (int)(Size > 48 ? Data[48] : 0);
  int srgb_intent2 = (int)(Size > 49 ? (int8_t)Data[49] : 0);
  int unit1 = (int)(Size > 50 ? Data[50] : 0);
  int unit2 = (int)(Size > 51 ? (int8_t)Data[51] : 0);

  png_set_gAMA(png_ptr, info_ptr, gamma1);
  png_set_gAMA_fixed(png_ptr, info_ptr, fixed_gamma1);

  double out_gamma = 0.0;
  (void)png_get_gAMA(png_ptr, info_ptr, &out_gamma);

  png_set_sRGB(png_ptr, info_ptr, srgb_intent1);
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent2);

  png_set_sCAL(png_ptr, info_ptr, unit1, width_d, height_d);
  png_set_sCAL(png_ptr, info_ptr, unit2, gamma2, out_gamma);
  png_set_sCAL_fixed(png_ptr, info_ptr, unit1, fixed_width, fixed_height);
  png_set_sCAL_fixed(png_ptr, info_ptr, unit2, fixed_gamma2, fixed_gamma1);

  png_set_gAMA(png_ptr, info_ptr, out_gamma);
  png_set_gAMA_fixed(png_ptr, info_ptr, fixed_gamma2);

  double out_gamma2 = 0.0;
  (void)png_get_gAMA(png_ptr, info_ptr, &out_gamma2);

  if (Size > 0) {
    int loops = (Data[0] % 8) + 1;
    for (int i = 0; i < loops; ++i) {
      int intent = (int)(Size > (size_t)(i + 1) ? (int8_t)Data[i + 1] : i);
      int unit = (int)(Size > (size_t)(i + 9) ? (int8_t)Data[i + 9] : i);
      double dg = make_double_from_bytes(Data, Size, (size_t)((i * 7) % (Size + 1)));
      png_fixed_point fg = (png_fixed_point)read_s32(Data, Size, (size_t)((i * 5) % (Size + 1)));

      png_set_sRGB(png_ptr, info_ptr, intent);
      png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, intent);
      png_set_gAMA(png_ptr, info_ptr, dg);
      png_set_gAMA_fixed(png_ptr, info_ptr, fg);
      png_set_sCAL(png_ptr, info_ptr, unit, dg, out_gamma2);
      png_set_sCAL_fixed(png_ptr, info_ptr, unit, fg, fixed_width);
      (void)png_get_gAMA(png_ptr, info_ptr, &out_gamma2);
    }
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}