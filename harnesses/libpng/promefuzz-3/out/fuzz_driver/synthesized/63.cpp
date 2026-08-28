// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_filter_heuristics at pngwrite.c:1124:1 in png.h
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
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  FILE *fp = reinterpret_cast<FILE*>(io_ptr);
  if (byteCountToRead == 0) return;

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "short read");
  }
}

extern "C" int LLVMFuzzerTestOneInput_63(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp == NULL) return 0;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);

  fp = fopen("./dummy_file", "rb");
  if (fp == NULL) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (info_ptr == NULL || end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_set_read_fn(png_ptr, fp, ReadDataFn);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_set_palette_to_rgb(png_ptr);
  png_set_swap(png_ptr);

  double weights[4] = {0.0, 0.5, 1.0, -1.0};
  double costs[4] = {1.0, 0.0, 2.0, 100.0};
  png_set_filter_heuristics(
      png_ptr,
      static_cast<int>(Size % 5),
      static_cast<int>(Size % 4),
      weights,
      costs);

  int alpha_modes[] = {
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
      0
  };

  double gammas[] = {
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
      1.0, 2.2, 0.45455, 0.0, -1.0
  };

  png_set_alpha_mode(
      png_ptr,
      alpha_modes[Size % (sizeof(alpha_modes) / sizeof(alpha_modes[0]))],
      gammas[(Size / 2) % (sizeof(gammas) / sizeof(gammas[0]))]);

  png_uint_32 filler = 0;
  if (Size >= 4) {
    filler = ((png_uint_32)Data[0] << 24) |
             ((png_uint_32)Data[1] << 16) |
             ((png_uint_32)Data[2] << 8) |
             (png_uint_32)Data[3];
  } else if (Size > 0) {
    filler = Data[0];
  }

  int filler_flag = (Size > 4 && (Data[4] & 1)) ? PNG_FILLER_AFTER : PNG_FILLER_BEFORE;
  png_set_filler(png_ptr, filler, filler_flag);

  png_read_update_info(png_ptr, info_ptr);

  png_start_read_image(png_ptr);
  if (Size > 0 && (Data[0] & 1)) {
    png_start_read_image(png_ptr);
  }

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes > 0 && height > 0) {
    png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
    if (row != NULL) {
      png_uint_32 max_rows = height;
      if (max_rows > 64) max_rows = 64;
      for (png_uint_32 y = 0; y < max_rows; ++y) {
        png_read_row(png_ptr, row, NULL);
      }
      free(row);
    }
  }

  if (height == 0 || width == 0 || rowbytes == 0) {
    png_read_end(png_ptr, end_info);
  } else {
    png_uint_32 consumed_rows = height > 64 ? 64 : height;
    for (png_uint_32 y = consumed_rows; y < height; ++y) {
      png_read_row(png_ptr, NULL, NULL);
    }
    png_read_end(png_ptr, end_info);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}