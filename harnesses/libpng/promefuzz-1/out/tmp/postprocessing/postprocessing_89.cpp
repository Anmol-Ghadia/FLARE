#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <png.h>

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
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : nullptr,
                            end_info ? &end_info : nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, &bit_depth, nullptr,
                     &interlace_method, nullptr, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, nullptr, &color_type,
                     nullptr, &compression_method, nullptr);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_oFFs);
#ifdef PNG_INFO_tIME
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_tIME);
#endif
#ifdef PNG_INFO_gAMA
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);
#endif

  png_uint_32 xppm = png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)xppm;

  png_uint_32 res_x = 0, res_y = 0;
  int phys_unit = 0;
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &phys_unit);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, &phys_unit);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  png_int_32 off_x = 0, off_y = 0;
  int offs_unit = 0;
  (void)png_get_oFFs(png_ptr, info_ptr, &off_x, &off_y, &offs_unit);

  png_fixed_point aspect = png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
  (void)aspect;

  png_read_end(png_ptr, end_info);

  (void)png_get_valid(png_ptr, end_info, PNG_INFO_pHYs);
  (void)png_get_valid(png_ptr, end_info, PNG_INFO_oFFs);
#ifdef PNG_INFO_tIME
  (void)png_get_valid(png_ptr, end_info, PNG_INFO_tIME);
#endif
#ifdef PNG_INFO_gAMA
  (void)png_get_valid(png_ptr, end_info, PNG_INFO_gAMA);
#endif
  (void)png_get_x_pixels_per_meter(png_ptr, end_info);
  (void)png_get_pHYs(png_ptr, end_info, &res_x, &res_y, &phys_unit);
  (void)png_get_oFFs(png_ptr, end_info, &off_x, &off_y, &offs_unit);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, end_info);
  (void)png_get_IHDR(png_ptr, end_info, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}