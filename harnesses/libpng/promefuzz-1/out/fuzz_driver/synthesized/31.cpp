// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_gamma at pngrtran.c:858:1 in png.h
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_alpha_mode_fixed at pngrtran.c:287:1 in png.h
// png_set_rgb_to_gray at pngrtran.c:1059:1 in png.h
// png_get_rgb_to_gray_status at pngget.c:1154:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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
#include <stdlib.h>
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 4 > size) return 0;
  uint32_t v = ((uint32_t)data[*off] << 24) |
               ((uint32_t)data[*off + 1] << 16) |
               ((uint32_t)data[*off + 2] << 8) |
               (uint32_t)data[*off + 3];
  *off += 4;
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t *off) {
  return (int32_t)ReadU32(data, size, off);
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = ReadU32(data, size, off);
  int selector = (int)(v % 12);
  switch (selector) {
    case 0: return 0.0;
    case 1: return 1.0;
    case 2: return 2.2;
    case 3: return 1.8;
    case 4: return -1.0;
    case 5: return -2.0;
    case 6: return 0.45455;
    case 7: return 10.0;
    case 8: return 100000.0;
    case 9: return -100000.0;
    case 10: return (double)((int32_t)v) / 100000.0;
    default: return (double)(v % 1000) / 100.0;
  }
}

static png_fixed_point ReadFixedLike(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = ReadU32(data, size, off);
  switch (v % 12) {
    case 0: return 0;
    case 1: return PNG_DEFAULT_sRGB;
    case 2: return PNG_GAMMA_MAC_18;
    case 3: return PNG_GAMMA_sRGB;
    case 4: return PNG_GAMMA_LINEAR;
    case 5: return 100000;
    case 6: return 220000;
    case 7: return -100000;
    case 8: return (png_fixed_point)ReadS32(data, size, off);
    case 9: return 1;
    case 10: return 2147483647;
    default: return -2147483647 - 1;
  }
}

extern "C" int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                               PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  size_t off = 0;

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  if (Size > 0) background.index = Data[off++ % Size];
  background.red = (png_uint_16)(ReadU32(Data, Size, &off) & 0xFFFFu);
#if defined(PNG_READ_BACKGROUND_SUPPORTED)
  background.green = (png_uint_16)(ReadU32(Data, Size, &off) & 0xFFFFu);
  background.blue = (png_uint_16)(ReadU32(Data, Size, &off) & 0xFFFFu);
  background.gray = (png_uint_16)(ReadU32(Data, Size, &off) & 0xFFFFu);
#endif

  png_set_expand(png_ptr);

#if defined(PNG_GAMMA_SUPPORTED)
  {
    double screen_gamma = ReadDoubleLike(Data, Size, &off);
    double file_gamma = ReadDoubleLike(Data, Size, &off);
    png_set_gamma(png_ptr, screen_gamma, file_gamma);
  }
#endif

#if defined(PNG_READ_ALPHA_MODE_SUPPORTED) || defined(PNG_GAMMA_SUPPORTED)
  {
    int mode_candidates[] = {
#ifdef PNG_ALPHA_PNG
      PNG_ALPHA_PNG,
#endif
#ifdef PNG_ALPHA_STANDARD
      PNG_ALPHA_STANDARD,
#endif
#ifdef PNG_ALPHA_OPTIMIZED
      PNG_ALPHA_OPTIMIZED,
#endif
#ifdef PNG_ALPHA_BROKEN
      PNG_ALPHA_BROKEN,
#endif
      0, 1, -1, 255
    };
    size_t idx = 0;
    if (Size > 0) idx = Data[off % Size] % (sizeof(mode_candidates) / sizeof(mode_candidates[0]));
    int mode = mode_candidates[idx];

    png_set_alpha_mode(png_ptr, mode, ReadDoubleLike(Data, Size, &off));
    png_set_alpha_mode_fixed(png_ptr, mode, ReadFixedLike(Data, Size, &off));
  }
#endif

#if defined(PNG_READ_RGB_TO_GRAY_SUPPORTED)
  {
    int error_action = (int)(ReadU32(Data, Size, &off) % 5u) - 2;
    double red = ReadDoubleLike(Data, Size, &off);
    double green = ReadDoubleLike(Data, Size, &off);
    png_set_rgb_to_gray(png_ptr, error_action, red, green);
    (void)png_get_rgb_to_gray_status(png_ptr);
  }
#endif

#if defined(PNG_READ_BACKGROUND_SUPPORTED)
  {
    int gamma_code_candidates[] = {
      PNG_BACKGROUND_GAMMA_UNKNOWN,
      PNG_BACKGROUND_GAMMA_SCREEN,
      PNG_BACKGROUND_GAMMA_FILE,
      PNG_BACKGROUND_GAMMA_UNIQUE,
      -1,
      99
    };
    size_t idx = 0;
    if (Size > 0) idx = Data[off % Size] % (sizeof(gamma_code_candidates) / sizeof(gamma_code_candidates[0]));
    int gamma_code = gamma_code_candidates[idx];
    int need_expand = (int)(ReadU32(Data, Size, &off) & 1u);
    double background_gamma = ReadDoubleLike(Data, Size, &off);
    png_set_background(png_ptr, &background, gamma_code, need_expand, background_gamma);
  }
#endif

#if defined(PNG_READ_SCALE_16_TO_8_SUPPORTED)
  png_set_scale_16(png_ptr);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}