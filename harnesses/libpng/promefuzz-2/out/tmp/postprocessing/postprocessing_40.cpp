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
#include <string.h>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);

  fp = fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);

  (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_meter(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_type = 0;
  if (png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                   &interlace_type, &compression_type, &filter_type)) {
    if (width > 0 && height > 0 && width < 4096 && height < 4096) {
      png_read_update_info(png_ptr, info_ptr);

      (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
      (void)png_get_pixels_per_inch(png_ptr, info_ptr);
      (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
      (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
      (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
      (void)png_get_pixels_per_meter(png_ptr, info_ptr);

      png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
      if (rowbytes > 0 && rowbytes < (1u << 20)) {
        png_bytep row = (png_bytep)png_malloc(png_ptr, rowbytes);
        if (row != nullptr) {
          for (png_uint_32 y = 0; y < height; ++y) {
            png_read_row(png_ptr, row, nullptr);

            (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
            (void)png_get_pixels_per_inch(png_ptr, info_ptr);
            (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
            (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
            (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
            (void)png_get_pixels_per_meter(png_ptr, info_ptr);
          }
          png_free(png_ptr, row);
        }
      } else {
        png_read_end(png_ptr, end_info);
      }
    }
  }

  (void)png_get_y_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pixels_per_inch(png_ptr, end_info);
  (void)png_get_y_pixels_per_inch(png_ptr, end_info);
  (void)png_get_x_pixels_per_inch(png_ptr, end_info);
  (void)png_get_x_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pixels_per_meter(png_ptr, end_info);

  if (!setjmp(png_jmpbuf(png_ptr))) {
    png_read_end(png_ptr, end_info);
  }

  (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_meter(png_ptr, info_ptr);

  (void)png_get_y_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pixels_per_inch(png_ptr, end_info);
  (void)png_get_y_pixels_per_inch(png_ptr, end_info);
  (void)png_get_x_pixels_per_inch(png_ptr, end_info);
  (void)png_get_x_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pixels_per_meter(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}