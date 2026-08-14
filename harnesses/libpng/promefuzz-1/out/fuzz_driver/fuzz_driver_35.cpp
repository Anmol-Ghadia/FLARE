// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_background_fixed at pngrtran.c:137:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_rgb_to_gray_fixed at pngrtran.c:970:1 in png.h
// png_get_rgb_to_gray_status at pngget.c:1154:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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

static png_fixed_point ReadFixed(const uint8_t *Data, size_t Size, size_t *Off) {
  if (*Off + 4 > Size) return 0;
  png_int_32 v = (png_int_32)(
      ((png_uint_32)Data[*Off] << 24) |
      ((png_uint_32)Data[*Off + 1] << 16) |
      ((png_uint_32)Data[*Off + 2] << 8) |
      ((png_uint_32)Data[*Off + 3]));
  *Off += 4;
  return v;
}

static int ReadInt(const uint8_t *Data, size_t Size, size_t *Off) {
  return (int)ReadFixed(Data, Size, Off);
}

static png_uint_32 ReadU32(const uint8_t *Data, size_t Size, size_t *Off) {
  if (*Off + 4 > Size) return 0;
  png_uint_32 v =
      ((png_uint_32)Data[*Off] << 24) |
      ((png_uint_32)Data[*Off + 1] << 16) |
      ((png_uint_32)Data[*Off + 2] << 8) |
      ((png_uint_32)Data[*Off + 3]);
  *Off += 4;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  size_t Off = 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
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

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  if (Size > Off) background.index = Data[Off++];
  background.red = (png_uint_16)(ReadU32(Data, Size, &Off) & 0xFFFFu);
#if defined(PNG_COLOR_16_SUPPORTED) || 1
  background.green = (png_uint_16)(ReadU32(Data, Size, &Off) & 0xFFFFu);
  background.blue = (png_uint_16)(ReadU32(Data, Size, &Off) & 0xFFFFu);
  background.gray = (png_uint_16)(ReadU32(Data, Size, &Off) & 0xFFFFu);
#endif

  int alpha_modes[] = {
#ifdef PNG_ALPHA_PNG
      PNG_ALPHA_PNG,
#endif
#ifdef PNG_ALPHA_STANDARD
      PNG_ALPHA_STANDARD,
#endif
#ifdef PNG_ALPHA_ASSOCIATED
      PNG_ALPHA_ASSOCIATED,
#endif
#ifdef PNG_ALPHA_OPTIMIZED
      PNG_ALPHA_OPTIMIZED,
#endif
#ifdef PNG_ALPHA_BROKEN
      PNG_ALPHA_BROKEN,
#endif
      ReadInt(Data, Size, &Off)
  };

  double gamma_values[] = {
      -1.0, 0.0, 1.0, 1.8, 2.2, 10.0,
#ifdef PNG_DEFAULT_sRGB
      (double)PNG_DEFAULT_sRGB,
#endif
#ifdef PNG_GAMMA_MAC_18
      (double)PNG_GAMMA_MAC_18,
#endif
#ifdef PNG_GAMMA_sRGB
      (double)PNG_GAMMA_sRGB / 100000.0,
#endif
#ifdef PNG_GAMMA_LINEAR
      (double)PNG_GAMMA_LINEAR / 100000.0,
#endif
      (double)ReadFixed(Data, Size, &Off) / 100000.0
  };

  int bg_gamma_codes[] = {
#ifdef PNG_BACKGROUND_GAMMA_UNKNOWN
      PNG_BACKGROUND_GAMMA_UNKNOWN,
#endif
#ifdef PNG_BACKGROUND_GAMMA_SCREEN
      PNG_BACKGROUND_GAMMA_SCREEN,
#endif
#ifdef PNG_BACKGROUND_GAMMA_FILE
      PNG_BACKGROUND_GAMMA_FILE,
#endif
#ifdef PNG_BACKGROUND_GAMMA_UNIQUE
      PNG_BACKGROUND_GAMMA_UNIQUE,
#endif
      ReadInt(Data, Size, &Off)
  };

  int need_expand_vals[] = {0, 1, ReadInt(Data, Size, &Off)};

  int error_actions[] = {
#ifdef PNG_ERROR_ACTION_NONE
      PNG_ERROR_ACTION_NONE,
#endif
#ifdef PNG_ERROR_ACTION_WARN
      PNG_ERROR_ACTION_WARN,
#endif
#ifdef PNG_ERROR_ACTION_ERROR
      PNG_ERROR_ACTION_ERROR,
#endif
      0, 1, 2, ReadInt(Data, Size, &Off)
  };

  int filler_flags[] = {
#ifdef PNG_FILLER_BEFORE
      PNG_FILLER_BEFORE,
#endif
#ifdef PNG_FILLER_AFTER
      PNG_FILLER_AFTER,
#endif
      ReadInt(Data, Size, &Off)
  };

  png_fixed_point fixed_vals[] = {
      0,
      1,
      100000,
      180000,
      220000,
      -1,
      -100000,
      ReadFixed(Data, Size, &Off),
      ReadFixed(Data, Size, &Off)
  };

  png_uint_32 filler = ReadU32(Data, Size, &Off);

  for (size_t i = 0; i < sizeof(alpha_modes) / sizeof(alpha_modes[0]); ++i) {
    for (size_t j = 0; j < sizeof(gamma_values) / sizeof(gamma_values[0]); ++j) {
      png_set_alpha_mode(png_ptr, alpha_modes[i], gamma_values[j]);
    }
  }

  for (size_t i = 0; i < sizeof(bg_gamma_codes) / sizeof(bg_gamma_codes[0]); ++i) {
    for (size_t j = 0; j < sizeof(need_expand_vals) / sizeof(need_expand_vals[0]); ++j) {
      for (size_t k = 0; k < sizeof(fixed_vals) / sizeof(fixed_vals[0]); ++k) {
        png_set_background_fixed(png_ptr, &background, bg_gamma_codes[i],
                                 need_expand_vals[j], fixed_vals[k]);
      }
    }
  }

  for (size_t i = 0; i < sizeof(error_actions) / sizeof(error_actions[0]); ++i) {
    for (size_t r = 0; r < sizeof(fixed_vals) / sizeof(fixed_vals[0]); ++r) {
      for (size_t g = 0; g < sizeof(fixed_vals) / sizeof(fixed_vals[0]); ++g) {
        png_set_rgb_to_gray_fixed(png_ptr, error_actions[i], fixed_vals[r], fixed_vals[g]);
        (void)png_get_rgb_to_gray_status(png_ptr);
      }
    }
  }

  for (size_t i = 0; i < sizeof(fixed_vals) / sizeof(fixed_vals[0]); ++i) {
    for (size_t j = 0; j < sizeof(fixed_vals) / sizeof(fixed_vals[0]); ++j) {
      png_set_gamma_fixed(png_ptr, fixed_vals[i], fixed_vals[j]);
    }
  }

  for (size_t i = 0; i < sizeof(bg_gamma_codes) / sizeof(bg_gamma_codes[0]); ++i) {
    for (size_t j = 0; j < sizeof(need_expand_vals) / sizeof(need_expand_vals[0]); ++j) {
      for (size_t k = 0; k < sizeof(gamma_values) / sizeof(gamma_values[0]); ++k) {
        png_set_background(png_ptr, &background, bg_gamma_codes[i],
                           need_expand_vals[j], gamma_values[k]);
      }
    }
  }

  for (size_t i = 0; i < sizeof(filler_flags) / sizeof(filler_flags[0]); ++i) {
    png_set_filler(png_ptr, filler, filler_flags[i]);
    png_set_filler(png_ptr, 0, filler_flags[i]);
    png_set_filler(png_ptr, 0xFFFFFFFFu, filler_flags[i]);
  }

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}