// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_y_offset_inches_fixed at pngget.c:389:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
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

static void NoOpPngError(png_structp, png_const_charp) {
  std::abort();
}

static void NoOpPngWarning(png_structp, png_const_charp) {
}

static void WriteDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);
}

static void ExerciseGetters(png_structp png_ptr, png_infop info_ptr) {
  if (!png_ptr || !info_ptr) return;

  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_x_offset_microns(png_ptr, info_ptr);
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_y_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);

#ifdef PNG_pHYs_SUPPORTED
  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
#endif
}

extern "C" int LLVMFuzzerTestOneInput_19(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, NoOpPngError, NoOpPngWarning);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (!info_ptr || !end_info) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : nullptr,
                            end_info ? &end_info : nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  ExerciseGetters(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, end_info);

  if (Size >= 1) {
    png_int_32 xoff = 0;
    png_int_32 yoff = 0;
    int unit = (Data[0] & 1) ? PNG_OFFSET_PIXEL : PNG_OFFSET_MICROMETER;

    if (Size >= 9) {
      xoff = static_cast<png_int_32>(
          (static_cast<uint32_t>(Data[1]) << 24) |
          (static_cast<uint32_t>(Data[2]) << 16) |
          (static_cast<uint32_t>(Data[3]) << 8) |
          static_cast<uint32_t>(Data[4]));
      yoff = static_cast<png_int_32>(
          (static_cast<uint32_t>(Data[5]) << 24) |
          (static_cast<uint32_t>(Data[6]) << 16) |
          (static_cast<uint32_t>(Data[7]) << 8) |
          static_cast<uint32_t>(Data[8]));
    } else if (Size >= 3) {
      xoff = static_cast<png_int_32>(static_cast<int8_t>(Data[1]));
      yoff = static_cast<png_int_32>(static_cast<int8_t>(Data[2]));
    }

#ifdef PNG_oFFs_SUPPORTED
    png_set_oFFs(png_ptr, info_ptr, xoff, yoff, unit);
    ExerciseGetters(png_ptr, info_ptr);

    png_set_oFFs(png_ptr, end_info, yoff, xoff,
                 unit == PNG_OFFSET_PIXEL ? PNG_OFFSET_MICROMETER : PNG_OFFSET_PIXEL);
    ExerciseGetters(png_ptr, end_info);
#endif
  }

#ifdef PNG_pHYs_SUPPORTED
  if (Size >= 4) {
    png_uint_32 res_x =
        (Size >= 8)
            ? ((static_cast<png_uint_32>(Data[0]) << 24) |
               (static_cast<png_uint_32>(Data[1]) << 16) |
               (static_cast<png_uint_32>(Data[2]) << 8) |
               static_cast<png_uint_32>(Data[3]))
            : static_cast<png_uint_32>(Data[0]);

    png_uint_32 res_y =
        (Size >= 8)
            ? ((static_cast<png_uint_32>(Data[4]) << 24) |
               (static_cast<png_uint_32>(Data[5]) << 16) |
               (static_cast<png_uint_32>(Data[6]) << 8) |
               static_cast<png_uint_32>(Data[7]))
            : static_cast<png_uint_32>(Data[1]);

    int phys_unit = (Size > 8 && (Data[8] & 1)) ? PNG_RESOLUTION_METER : PNG_RESOLUTION_UNKNOWN;

    png_set_pHYs(png_ptr, info_ptr, res_x, res_y, phys_unit);
    ExerciseGetters(png_ptr, info_ptr);

    png_set_pHYs(png_ptr, end_info, res_y, res_x,
                 phys_unit == PNG_RESOLUTION_METER ? PNG_RESOLUTION_UNKNOWN
                                                   : PNG_RESOLUTION_METER);
    ExerciseGetters(png_ptr, end_info);
  }
#endif

  ExerciseGetters(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}