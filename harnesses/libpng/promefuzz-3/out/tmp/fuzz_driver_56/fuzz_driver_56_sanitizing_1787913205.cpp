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
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_inforp info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_byte sig[8];
    std::memcpy(sig, Data, 8);
    if (png_sig_cmp(sig, 0, 8) == 0) {
      png_set_sig_bytes(png_ptr, 0);
    }
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_method = 0;
  int compression_method = 0;
  int filter_method = 0;

  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, &bit_depth, nullptr,
                     &interlace_method, nullptr, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, nullptr, &color_type,
                     nullptr, &compression_method, nullptr);

  (void)png_get_bit_depth(png_ptr, info_ptr);
  (void)png_get_image_width(png_ptr, info_ptr);

  png_const_bytep sig_ptr = png_get_signature(png_ptr, info_ptr);
  if (sig_ptr != nullptr) {
    volatile png_byte b = sig_ptr[0];
    (void)b;
  }

  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

  png_bytepp rows = png_get_rows(png_ptr, info_ptr);
  if (rows != nullptr && height > 0 && rows[0] != nullptr) {
    volatile png_byte first = rows[0][0];
    (void)first;
  }

  (void)png_get_rows(png_ptr, info_ptr);
  (void)png_get_bit_depth(png_ptr, info_ptr);
  (void)png_get_image_width(png_ptr, info_ptr);
  (void)png_get_signature(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  std::fclose(fp);
  return 0;
}