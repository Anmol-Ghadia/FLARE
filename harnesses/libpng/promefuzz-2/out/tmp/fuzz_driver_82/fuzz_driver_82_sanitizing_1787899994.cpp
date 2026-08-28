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
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (!info_ptr || !end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  if (Size > 0) {
    if (Data[0] & 0x01) png_set_bgr(png_ptr);
    if (Data[0] & 0x02) png_set_expand(png_ptr);
    if (Data[0] & 0x04) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (Data[0] & 0x08) png_set_expand_16(png_ptr);
    if (Data[0] & 0x10) png_set_palette_to_rgb(png_ptr);
  }

  png_read_info(png_ptr, info_ptr);

  if (Size > 1) {
    if (Data[1] & 0x01) png_set_bgr(png_ptr);
    if (Data[1] & 0x02) png_set_expand(png_ptr);
    if (Data[1] & 0x04) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (Data[1] & 0x08) png_set_expand_16(png_ptr);
    if (Data[1] & 0x10) png_set_palette_to_rgb(png_ptr);
  }

  if (Size > 2 && (Data[2] & 0x01)) {
    png_read_update_info(png_ptr, info_ptr);
  }

  png_start_read_image(png_ptr);
  if (Size > 2 && (Data[2] & 0x02)) {
    png_start_read_image(png_ptr);
  }

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes > 0 && width > 0 && height > 0) {
    const png_uint_32 max_rows = 64;
    png_uint_32 rows_to_read = height < max_rows ? height : max_rows;

    std::vector<png_byte> row(rowbytes);
    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }

    if (rows_to_read == height) {
      png_read_end(png_ptr, end_info);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}