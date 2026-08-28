// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_hIST at pngget.c:808:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_hIST at pngget.c:808:1 in png.h
// png_get_hIST at pngget.c:808:1 in png.h
// png_get_hIST at pngget.c:808:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);

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

  png_infop end_info = png_create_info_struct(png_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (end_info != nullptr) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    } else {
      png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    }
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(Data), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  (void)png_get_image_width(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio(png_ptr, info_ptr);

  png_uint_16p hist = nullptr;
  (void)png_get_hIST(png_ptr, info_ptr, &hist);

  (void)png_get_image_width(nullptr, info_ptr);
  (void)png_get_image_width(png_ptr, nullptr);
  (void)png_get_y_pixels_per_meter(nullptr, info_ptr);
  (void)png_get_y_pixels_per_meter(png_ptr, nullptr);
  (void)png_get_pixels_per_inch(nullptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, nullptr);
  (void)png_get_pixel_aspect_ratio(nullptr, info_ptr);
  (void)png_get_pixel_aspect_ratio(png_ptr, nullptr);
  (void)png_get_hIST(nullptr, info_ptr, &hist);
  (void)png_get_hIST(png_ptr, nullptr, &hist);
  (void)png_get_hIST(png_ptr, info_ptr, nullptr);

  if (end_info != nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  } else {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  }
  std::fclose(fp);
  return 0;
}