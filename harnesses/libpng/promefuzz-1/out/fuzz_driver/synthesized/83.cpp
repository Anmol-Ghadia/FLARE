// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
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

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read beyond end");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

extern "C" int LLVMFuzzerTestOneInput_83(const uint8_t *Data, size_t Size) {
  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState st = {Data, Size, 0};

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
    return 0;

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

  png_set_read_fn(png_ptr, &st, PngReadCallback);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0, compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, NULL, &bit_depth, NULL, &interlace_method, NULL, &filter_method);

  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, NULL, NULL, NULL);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, NULL, &unit_type);

  png_colorp palette = NULL;
  int num_palette = -1;
  (void)png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
  (void)png_get_PLTE(png_ptr, info_ptr, &palette, NULL);

  int scal_unit = 0;
  png_fixed_point scal_width = 0, scal_height = 0;
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &scal_unit, &scal_width, &scal_height);
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, NULL, NULL, NULL);
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &scal_unit, NULL, &scal_height);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_IDAT);
  (void)png_get_valid(png_ptr, info_ptr, 0xffffffffU);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}