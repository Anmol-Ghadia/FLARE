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
#include <vector>
#include <fstream>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "NULL io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read beyond end");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void ExerciseGetters(png_structp png_ptr, png_infop info_ptr) {
  (void)png_get_image_width(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_pixels_per_meter(png_ptr, info_ptr);

  (void)png_get_image_width(NULL, info_ptr);
  (void)png_get_image_width(png_ptr, NULL);
  (void)png_get_pixel_aspect_ratio_fixed(NULL, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, NULL);
  (void)png_get_y_pixels_per_meter(NULL, info_ptr);
  (void)png_get_y_pixels_per_meter(png_ptr, NULL);
  (void)png_get_pixels_per_inch(NULL, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, NULL);
  (void)png_get_x_pixels_per_inch(NULL, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, NULL);
  (void)png_get_pixels_per_meter(NULL, info_ptr);
  (void)png_get_pixels_per_meter(png_ptr, NULL);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs) {
      ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

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
    ExerciseGetters(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_read_info(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);
  int interlace_type = png_get_interlace_type(png_ptr, info_ptr);

  if (bit_depth == 16) png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
  if (interlace_type != PNG_INTERLACE_NONE) png_set_interlace_handling(png_ptr);

  png_read_update_info(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (width > 0 && height > 0 && rowbytes > 0) {
    const png_uint_32 kMaxDim = 4096;
    const png_size_t kMaxRowbytes = 1 << 20;
    const png_uint_32 kMaxRows = 4096;

    if (width <= kMaxDim && height <= kMaxRows && rowbytes <= kMaxRowbytes) {
      std::vector<png_byte> image;
      std::vector<png_bytep> rows;

      if (rowbytes <= SIZE_MAX / height) {
        image.resize(rowbytes * height);
        rows.resize(height);
        for (png_uint_32 y = 0; y < height; ++y) {
          rows[y] = image.data() + y * rowbytes;
        }
        png_read_image(png_ptr, rows.data());
      }
    }
  }

  ExerciseGetters(png_ptr, info_ptr);
  png_read_end(png_ptr, end_info);
  ExerciseGetters(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}