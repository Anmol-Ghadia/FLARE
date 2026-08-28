// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_gamma at pngrtran.c:858:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_set_alpha_mode at pngrtran.c:400:1 in png.h
// png_set_alpha_mode_fixed at pngrtran.c:287:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_background at pngrtran.c:167:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* st = reinterpret_cast<BufferState*>(io_ptr);
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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                               PngErrorCallback, PngWarningCallback);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  BufferState st = {Data, Size, 0};
  png_set_read_fn(png_ptr, &st, PngReadCallback);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_color_16 bg;
  memset(&bg, 0, sizeof(bg));
  if (Size > 0) bg.index = Data[0];
  if (Size > 2) bg.red = (png_uint_16)((Data[1] << 8) | Data[2]);
  if (Size > 4) bg.green = (png_uint_16)((Data[3] << 8) | Data[4]);
  if (Size > 6) bg.blue = (png_uint_16)((Data[5] << 8) | Data[6]);
  if (Size > 8) bg.gray = (png_uint_16)((Data[7] << 8) | Data[8]);

  png_set_expand(png_ptr);

  int mode = PNG_ALPHA_PNG;
#ifdef PNG_ALPHA_STANDARD
  if (Size > 9 && (Data[9] & 1)) mode = PNG_ALPHA_STANDARD;
#endif
#ifdef PNG_ALPHA_ASSOCIATED
  if (Size > 9 && (Data[9] & 2)) mode = PNG_ALPHA_ASSOCIATED;
#endif
#ifdef PNG_ALPHA_OPTIMIZED
  if (Size > 9 && (Data[9] & 4)) mode = PNG_ALPHA_OPTIMIZED;
#endif
#ifdef PNG_ALPHA_BROKEN
  if (Size > 9 && (Data[9] & 8)) mode = PNG_ALPHA_BROKEN;
#endif

  double screen_gamma = 2.2;
  double file_gamma = 0.45455;
  double output_gamma = 2.2;
  if (Size > 17) {
    uint32_t a = (uint32_t(Data[10]) << 24) | (uint32_t(Data[11]) << 16) |
                 (uint32_t(Data[12]) << 8) | uint32_t(Data[13]);
    uint32_t b = (uint32_t(Data[14]) << 24) | (uint32_t(Data[15]) << 16) |
                 (uint32_t(Data[16]) << 8) | uint32_t(Data[17]);
    screen_gamma = (a % 4000) / 1000.0;
    file_gamma = (b % 4000) / 1000.0;
    output_gamma = ((a ^ b) % 4000) / 1000.0;
  }

  png_fixed_point fixed_screen = PNG_GAMMA_sRGB;
  png_fixed_point fixed_file = PNG_GAMMA_LINEAR;
  png_fixed_point fixed_output = PNG_GAMMA_sRGB;
  if (Size > 29) {
    fixed_screen = (png_fixed_point)((int32_t)((uint32_t(Data[18]) << 24) |
                                               (uint32_t(Data[19]) << 16) |
                                               (uint32_t(Data[20]) << 8) |
                                               uint32_t(Data[21])));
    fixed_file = (png_fixed_point)((int32_t)((uint32_t(Data[22]) << 24) |
                                             (uint32_t(Data[23]) << 16) |
                                             (uint32_t(Data[24]) << 8) |
                                             uint32_t(Data[25])));
    fixed_output = (png_fixed_point)((int32_t)((uint32_t(Data[26]) << 24) |
                                               (uint32_t(Data[27]) << 16) |
                                               (uint32_t(Data[28]) << 8) |
                                               uint32_t(Data[29])));
  }

  if (Size > 30) {
    switch (Data[30] % 4) {
      case 0:
        output_gamma = PNG_DEFAULT_sRGB;
        break;
      case 1:
        output_gamma = PNG_GAMMA_LINEAR / 100000.0;
        break;
      case 2:
        fixed_output = PNG_DEFAULT_sRGB;
        break;
      case 3:
        fixed_output = PNG_GAMMA_MAC_18;
        break;
    }
  }

  png_set_gamma(png_ptr, screen_gamma, file_gamma);
  png_set_gamma_fixed(png_ptr, fixed_screen, fixed_file);
  png_set_alpha_mode(png_ptr, mode, output_gamma);
  png_set_alpha_mode_fixed(png_ptr, mode, fixed_output);

  png_read_info(png_ptr, info_ptr);

  int bg_gamma_code = PNG_BACKGROUND_GAMMA_UNKNOWN;
  if (Size > 31) {
    switch (Data[31] % 4) {
      case 0: bg_gamma_code = PNG_BACKGROUND_GAMMA_UNKNOWN; break;
      case 1: bg_gamma_code = PNG_BACKGROUND_GAMMA_SCREEN; break;
      case 2: bg_gamma_code = PNG_BACKGROUND_GAMMA_FILE; break;
      case 3: bg_gamma_code = PNG_BACKGROUND_GAMMA_UNIQUE; break;
    }
  }
  int need_expand = (Size > 32) ? (Data[32] & 1) : 0;
  double bg_gamma = (Size > 33) ? ((Data[33] % 200) / 100.0) : 1.0;

  png_set_background(png_ptr, &bg, bg_gamma_code, need_expand, bg_gamma);
  png_set_expand(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 || height == 0) {
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  if (rowbytes > (1U << 20)) rowbytes = (1U << 20);
  if (height > 4096) height = 4096;

  png_bytep row = (png_bytep)malloc(rowbytes);
  if (row == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    png_read_row(png_ptr, row, NULL);
  }

  free(row);
  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}