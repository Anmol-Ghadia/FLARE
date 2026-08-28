// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_end at pngread.c:766:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_x_pixels_per_inch at pngget.c:355:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_x_pixels_per_inch at pngget.c:355:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_x_pixels_per_inch at pngget.c:355:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
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
#include <vector>

static void WriteToDummyFile(const uint8_t* data, size_t size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size > 0) std::fwrite(data, 1, size, fp);
  std::fclose(fp);
}

static void ReadAndQueryPng(FILE* fp) {
  if (!fp) return;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return;
  }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;

  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, nullptr);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  if (bit_depth == 16) png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height > 0 && rowbytes > 0 && rowbytes < (1u << 26) &&
      height < (1u << 20)) {
    std::vector<png_bytep> rows(height, nullptr);
    std::vector<uint8_t> image;
    if (rowbytes <= SIZE_MAX / height) {
      image.resize(rowbytes * height);
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + y * rowbytes;
      }
      png_read_image(png_ptr, rows.data());

      (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
      (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
      (void)png_get_pixels_per_meter(png_ptr, info_ptr);
      (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
      (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
      (void)png_get_pixels_per_inch(png_ptr, info_ptr);
    }
  }

  png_read_end(png_ptr, end_info);

  (void)png_get_pHYs(png_ptr, end_info, &res_x, &res_y, &unit_type);
  (void)png_get_x_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pixels_per_meter(png_ptr, end_info);
  (void)png_get_x_pixels_per_inch(png_ptr, end_info);
  (void)png_get_y_pixels_per_inch(png_ptr, end_info);
  (void)png_get_pixels_per_inch(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  (void)width;
}

extern "C" int LLVMFuzzerTestOneInput_29(const uint8_t* Data, size_t Size) {
  WriteToDummyFile(Data, Size);

  FILE* fp = std::fopen("./dummy_file", "rb");
  if (!fp) return 0;

  ReadAndQueryPng(fp);
  std::fclose(fp);

  return 0;
}