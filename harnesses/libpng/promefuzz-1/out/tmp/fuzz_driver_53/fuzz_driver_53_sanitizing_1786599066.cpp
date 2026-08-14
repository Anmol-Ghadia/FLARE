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
#include <vector>
#include <algorithm>

static void WriteToDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }
}

static void ReadPngWithLibpng(const uint8_t *Data, size_t Size) {
  WriteToDummyFile(Data, Size);

  FILE *fp = fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  int allowed = (Size > 0) ? (Data[0] & 1) : 0;
  png_set_check_for_invalid_index(png_ptr, allowed);

  png_set_expand(png_ptr);

  int option_base = 0;
#ifdef PNG_MAXIMUM_INFLATE_WINDOW
  option_base = PNG_MAXIMUM_INFLATE_WINDOW;
  png_set_option(png_ptr, PNG_MAXIMUM_INFLATE_WINDOW, (Size > 1) ? (Data[1] & 1) : 0);
#endif
#ifdef PNG_SKIP_sRGB_CHECK_PROFILE
  png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, (Size > 2) ? (Data[2] & 1) : 0);
#endif

  for (int i = 0; i < 4; ++i) {
    int option = option_base + ((i * 2) & 0x7ffffffe);
    int onoff = (Size > static_cast<size_t>(3 + i)) ? (Data[3 + i] & 1) : 0;
    (void)png_set_option(png_ptr, option, onoff);
  }

  png_read_info(png_ptr, info_ptr);

  (void)png_set_interlace_handling(png_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;

  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, &bit_depth, nullptr,
                     &interlace_method, nullptr, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, nullptr, &color_type,
                     nullptr, &compression_method, nullptr);

  (void)png_get_palette_max(png_ptr, info_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  png_uint_32 read_height = png_get_image_height(png_ptr, info_ptr);

  if (rowbytes > 0 && read_height > 0) {
    png_uint_32 max_rows = read_height;
    if (max_rows > 64)
      max_rows = 64;

    std::vector<png_byte> row(rowbytes);
    for (png_uint_32 y = 0; y < max_rows; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }
  }

  png_read_end(png_ptr, end_info);

  (void)png_get_palette_max(png_ptr, end_info);
  (void)png_get_IHDR(png_ptr, end_info, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp null_png = nullptr;
  png_infop null_info = nullptr;

  (void)png_set_option(null_png, 0, 0);
  (void)png_get_IHDR(null_png, null_info, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);
  (void)png_get_palette_max(null_png, null_info);
  png_set_check_for_invalid_index(null_png, 0);
  png_set_expand(null_png);
  (void)png_set_interlace_handling(null_png);

  ReadPngWithLibpng(Data, Size);
  return 0;
}