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
#include <vector>
#include <algorithm>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      if (Size > 0) {
        std::fwrite(Data, 1, Size, fp);
      }
      std::fclose(fp);
    }
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  FILE *fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  size_t pre_read = 0;
  if (Size > 0) {
    pre_read = Data[0] % 10;
    pre_read = std::min(pre_read, Size);
  }

  if (pre_read > 0) {
    uint8_t tmp[8];
    std::fread(tmp, 1, pre_read, fp);
  }

  png_init_io(png_ptr, fp);

  png_set_sig_bytes(png_ptr, static_cast<int>(pre_read));
  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  png_read_info(png_ptr, info_ptr);

  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  if (Size > 1 && (Data[1] & 1)) {
    png_set_expand(png_ptr);
  }
  if (Size > 2 && (Data[2] & 1)) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (Size > 3 && (Data[3] & 1)) {
    png_set_strip_16(png_ptr);
  }
  if (Size > 4 && (Data[4] & 1)) {
    png_set_packing(png_ptr);
  }
  if (Size > 5 && (Data[5] & 1)) {
    png_set_tRNS_to_alpha(png_ptr);
  }
  if (Size > 6 && (Data[6] & 1)) {
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes > 0 && rowbytes < (1u << 26) && height < (1u << 20)) {
    std::vector<png_byte> row(rowbytes);
    std::vector<png_byte> display_row;
    png_bytep display_ptr = nullptr;

    if (Size > 7 && (Data[7] & 1)) {
      display_row.resize(rowbytes);
      display_ptr = display_row.data();
    }

    png_uint_32 rows_to_read = height;
    if (Size > 8) {
      rows_to_read = std::min<png_uint_32>(height, static_cast<png_uint_32>(Data[8]));
    }

    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      (void)png_get_io_state(png_ptr);
      (void)png_get_io_chunk_type(png_ptr);
      png_read_row(png_ptr, row.data(), display_ptr);
    }
  }

  std::fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}