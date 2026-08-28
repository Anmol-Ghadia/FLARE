// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_add_alpha at pngtrans.c:205:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_oFFs at pngget.c:870:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
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

  png_int_32 offset_x = 0;
  png_int_32 offset_y = 0;
  int unit_type = 0;
  png_uint_32 res_x = 0;
  png_uint_32 res_y = 0;
  int phys_unit_type = 0;

  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, &unit_type);
  (void)png_get_oFFs(png_ptr, info_ptr, nullptr, &offset_y, &unit_type);
  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, nullptr, &unit_type);
  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, nullptr);
  (void)png_get_oFFs(nullptr, info_ptr, &offset_x, &offset_y, &unit_type);
  (void)png_get_oFFs(png_ptr, nullptr, &offset_x, &offset_y, &unit_type);

  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_x_offset_microns(png_ptr, info_ptr);

#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_pHYs_SUPPORTED
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &phys_unit_type);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &res_y, &phys_unit_type);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, nullptr, &phys_unit_type);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, nullptr);
#endif

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  if (bit_depth == 16) png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
  if (!(color_type & PNG_COLOR_MASK_ALPHA)) png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  (void)png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, &unit_type);
  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &phys_unit_type);
#endif

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height > 0 && rowbytes > 0 && rowbytes < (1u << 26) && height < (1u << 20)) {
    png_bytep row = static_cast<png_bytep>(std::malloc(rowbytes));
    if (row) {
      for (png_uint_32 y = 0; y < height; ++y) {
        png_read_row(png_ptr, row, nullptr);
        if ((y & 1u) == 0) {
          (void)png_get_x_offset_pixels(png_ptr, info_ptr);
          (void)png_get_y_offset_pixels(png_ptr, info_ptr);
          (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
          (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
          (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif
        }
      }
      std::free(row);
    }
  }

  png_read_end(png_ptr, end_info);

  (void)png_get_oFFs(png_ptr, end_info, &offset_x, &offset_y, &unit_type);
  (void)png_get_x_offset_pixels(png_ptr, end_info);
  (void)png_get_y_offset_pixels(png_ptr, end_info);
  (void)png_get_x_offset_microns(png_ptr, end_info);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, end_info);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, end_info);
#endif
#ifdef PNG_pHYs_SUPPORTED
  (void)png_get_pHYs_dpi(png_ptr, end_info, &res_x, &res_y, &phys_unit_type);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  (void)width;
  return 0;
}