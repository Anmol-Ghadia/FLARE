// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_valid at pngget.c:20:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <png.h>

static void ReadPngAndQuery(const uint8_t* data, size_t size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return;
  }

  if (size > 0) {
    (void)std::fwrite(data, 1, size, fp);
  }
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);

  png_int_32 offset_x = 0, offset_y = 0;
  int unit_type = 0;
  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, &unit_type);
  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, nullptr, &unit_type);
  (void)png_get_oFFs(png_ptr, info_ptr, nullptr, &offset_y, nullptr);

  (void)png_get_x_offset_pixels(png_ptr, info_ptr);

  png_uint_32 res_x = 0, res_y = 0;
  int phys_unit = 0;
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &phys_unit);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, &phys_unit);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);

  png_uint_32 dpi_x = 0, dpi_y = 0;
  int dpi_unit = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &dpi_x, &dpi_y, &dpi_unit);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &dpi_x, nullptr, &dpi_unit);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &dpi_y, nullptr);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_oFFs);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);
  (void)png_get_valid(png_ptr, info_ptr, 0xffffffffU);

  png_read_end(png_ptr, end_info);

  (void)png_get_valid(png_ptr, end_info, PNG_INFO_oFFs);
  (void)png_get_valid(png_ptr, end_info, PNG_INFO_pHYs);
  (void)png_get_IHDR(png_ptr, end_info, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_oFFs(png_ptr, end_info, &offset_x, &offset_y, &unit_type);
  (void)png_get_x_offset_pixels(png_ptr, end_info);
  (void)png_get_pHYs(png_ptr, end_info, &res_x, &res_y, &phys_unit);
  (void)png_get_pHYs_dpi(png_ptr, end_info, &dpi_x, &dpi_y, &dpi_unit);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
  ReadPngAndQuery(Data, Size);

  (void)png_get_valid(nullptr, nullptr, PNG_INFO_oFFs);
  (void)png_get_IHDR(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);
  (void)png_get_oFFs(nullptr, nullptr, nullptr, nullptr, nullptr);
  (void)png_get_x_offset_pixels(nullptr, nullptr);
  (void)png_get_pHYs(nullptr, nullptr, nullptr, nullptr, nullptr);
  (void)png_get_pHYs_dpi(nullptr, nullptr, nullptr, nullptr, nullptr);

  return 0;
}