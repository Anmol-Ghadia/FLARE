// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_background_fixed at pngrtran.c:137:1 in png.h
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_alpha_mode_fixed at pngrtran.c:287:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_background_fixed at pngrtran.c:137:1 in png.h
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_alpha_mode_fixed at pngrtran.c:287:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = static_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_17(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (Size == 0) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorCallback, PngWarningCallback);
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

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  if (Size > 0) background.index = Data[0];
  if (Size > 2) background.red = (png_uint_16)((Data[1] << 8) | Data[2]);
  if (Size > 4) background.green = (png_uint_16)((Data[3] << 8) | Data[4]);
  if (Size > 6) background.blue = (png_uint_16)((Data[5] << 8) | Data[6]);
  if (Size > 8) background.gray = (png_uint_16)((Data[7] << 8) | Data[8]);

  png_fixed_point gamma1 = 100000;
  png_fixed_point gamma2 = 100000;
  png_fixed_point bg_gamma = 100000;
  png_fixed_point alpha_gamma = PNG_GAMMA_sRGB;

  if (Size > 12) {
    gamma1 = (png_fixed_point)(
        ((uint32_t)Data[9] << 24) |
        ((uint32_t)Data[10] << 16) |
        ((uint32_t)Data[11] << 8) |
        (uint32_t)Data[12]);
  }

  if (Size > 16) {
    gamma2 = (png_fixed_point)(
        ((uint32_t)Data[13] << 24) |
        ((uint32_t)Data[14] << 16) |
        ((uint32_t)Data[15] << 8) |
        (uint32_t)Data[16]);
  }

  if (Size > 20) {
    bg_gamma = (png_fixed_point)(
        ((uint32_t)Data[17] << 24) |
        ((uint32_t)Data[18] << 16) |
        ((uint32_t)Data[19] << 8) |
        (uint32_t)Data[20]);
  }

  if (Size > 24) {
    alpha_gamma = (png_fixed_point)(
        ((uint32_t)Data[21] << 24) |
        ((uint32_t)Data[22] << 16) |
        ((uint32_t)Data[23] << 8) |
        (uint32_t)Data[24]);
  }

  int background_gamma_code = PNG_BACKGROUND_GAMMA_UNKNOWN;
  int need_expand = 0;
  int alpha_mode = 0;
  int flush_rows = 0;

  if (Size > 25) background_gamma_code = Data[25] % 6;
  if (Size > 26) need_expand = Data[26] & 1;
  if (Size > 27) alpha_mode = (int)((int8_t)Data[27]);
  if (Size > 28) flush_rows = (int)Data[28];

  switch (Size > 29 ? (Data[29] % 4) : 0) {
    case 0: alpha_gamma = PNG_DEFAULT_sRGB; break;
    case 1: alpha_gamma = PNG_GAMMA_MAC_18; break;
    case 2: alpha_gamma = PNG_GAMMA_sRGB; break;
    case 3: alpha_gamma = PNG_GAMMA_LINEAR; break;
  }

  png_set_tRNS_to_alpha(png_ptr);

  png_set_gamma_fixed(png_ptr, gamma1, gamma2);
  png_set_background(png_ptr, &background, background_gamma_code, need_expand, 2.2);
  png_set_background_fixed(png_ptr, &background, background_gamma_code, need_expand, bg_gamma);
  png_set_alpha_mode(png_ptr, alpha_mode, 2.2);
  png_set_alpha_mode_fixed(png_ptr, alpha_mode, alpha_gamma);

#ifdef PNG_WRITE_FLUSH_SUPPORTED
  png_set_flush(png_ptr, flush_rows);
#endif

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#endif

  png_read_info(png_ptr, info_ptr);

  png_set_tRNS_to_alpha(png_ptr);

  png_set_gamma_fixed(png_ptr, gamma1 == 0 ? 100000 : gamma1, gamma2);
  png_set_background(png_ptr, &background, background_gamma_code, need_expand ^ 1, 1.0);
  png_set_background_fixed(png_ptr, &background, background_gamma_code, need_expand, bg_gamma == 0 ? 100000 : bg_gamma);
  png_set_alpha_mode(png_ptr, alpha_mode ^ 1, 1.0);
  png_set_alpha_mode_fixed(png_ptr, alpha_mode, alpha_gamma);

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#endif

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes > 0 && height > 0) {
    png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
    if (row != NULL) {
      png_uint_32 limit = height;
      if (limit > 64) limit = 64;
      for (png_uint_32 y = 0; y < limit; ++y) {
        png_read_row(png_ptr, row, NULL);
      }
      free(row);
    }
  }

  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}