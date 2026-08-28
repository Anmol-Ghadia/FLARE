// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_read_png at pngread.c:1049:1 in png.h
// png_get_rows at pngget.c:40:1 in png.h
// png_get_rows at pngget.c:40:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_56(const uint8_t *Data, size_t Size) {
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

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_infop info_to_destroy = info_ptr;
    png_destroy_read_struct(&png_ptr, &info_to_destroy, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

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

  png_infop info_to_destroy = info_ptr;
  png_destroy_read_struct(&png_ptr, &info_to_destroy, nullptr);
  std::fclose(fp);
  return 0;
}