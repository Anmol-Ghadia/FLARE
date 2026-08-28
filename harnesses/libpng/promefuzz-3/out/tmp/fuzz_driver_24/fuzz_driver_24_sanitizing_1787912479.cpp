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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) return 0;

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_uint_16p hist = nullptr;
  png_color_16p background = nullptr;
  int unit = 0;
  char* swidth = nullptr;
  char* sheight = nullptr;
  int srgb_intent = 0;
  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;

  png_get_valid(png_ptr, info_ptr, PNG_INFO_hIST);
  png_get_hIST(png_ptr, info_ptr, &hist);
  png_get_bKGD(png_ptr, info_ptr, &background);
  png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
  png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);

  png_read_info(png_ptr, info_ptr);

  png_get_valid(png_ptr, info_ptr, PNG_INFO_hIST);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);

  png_get_hIST(png_ptr, info_ptr, &hist);
  png_get_bKGD(png_ptr, info_ptr, &background);
  png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
  png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);

  if (width > 0 && height > 0) {
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes > 0 && rowbytes < (1u << 26) && height < (1u << 20)) {
      png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
      if (row != nullptr) {
        for (png_uint_32 y = 0; y < height; ++y) {
          png_read_row(png_ptr, row, nullptr);

          png_get_hIST(png_ptr, info_ptr, &hist);
          png_get_bKGD(png_ptr, info_ptr, &background);
          png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
          png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
          png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
        }
        free(row);
      }
    } else {
      png_read_end(png_ptr, end_info);
    }
  }

  png_get_hIST(png_ptr, info_ptr, &hist);
  png_get_bKGD(png_ptr, info_ptr, &background);
  png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
  png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);

  png_get_valid(png_ptr, end_info, PNG_INFO_hIST);
  png_get_hIST(png_ptr, end_info, &hist);
  png_get_bKGD(png_ptr, end_info, &background);
  png_get_sCAL_s(png_ptr, end_info, &unit, &swidth, &sheight);
  png_get_sRGB(png_ptr, end_info, &srgb_intent);
  png_get_pHYs_dpi(png_ptr, end_info, &res_x, &res_y, &unit_type);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}