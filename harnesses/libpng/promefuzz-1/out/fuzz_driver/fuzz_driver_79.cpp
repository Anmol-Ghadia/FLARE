// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_get_x_pixels_per_inch at pngget.c:355:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
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
// png_read_end at pngread.c:766:1 in png.h
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
#include <stdio.h>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void WriteDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void QueryPhysAPIs(png_structp png_ptr, png_infop info_ptr) {
  if (!png_ptr || !info_ptr) return;

  png_uint_32 rx = 0, ry = 0;
  int unit = 0;

  (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_pixels_per_inch(png_ptr, info_ptr);
  (void)png_get_pHYs(png_ptr, info_ptr, &rx, &ry, &unit);
  (void)png_get_pHYs(png_ptr, info_ptr, &rx, &ry, nullptr);
  (void)png_get_pHYs(png_ptr, info_ptr, &rx, nullptr, &unit);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &ry, &unit);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  rx = ry = 0;
  unit = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, &ry, &unit);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, &ry, nullptr);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, nullptr, &unit);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &ry, &unit);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, nullptr, nullptr);
}

static void FuzzReadPath(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  FILE *fp = fopen("./dummy_file", "rb");
  if (!fp) return;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);
  QueryPhysAPIs(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_type, &compression_type, &filter_method);

  png_read_end(png_ptr, end_info);
  QueryPhysAPIs(png_ptr, info_ptr);
  QueryPhysAPIs(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
}

static void FuzzManualInfoPath(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return;
  }

  png_uint_32 x = 0, y = 0;
  int unit = PNG_RESOLUTION_UNKNOWN;

  if (Size >= 4) {
    x = ((png_uint_32)Data[0] << 24) |
        ((png_uint_32)Data[1] << 16) |
        ((png_uint_32)Data[2] << 8) |
        (png_uint_32)Data[3];
  }
  if (Size >= 8) {
    y = ((png_uint_32)Data[4] << 24) |
        ((png_uint_32)Data[5] << 16) |
        ((png_uint_32)Data[6] << 8) |
        (png_uint_32)Data[7];
  }
  if (Size >= 9) {
    unit = (Data[8] & 1) ? PNG_RESOLUTION_METER : PNG_RESOLUTION_UNKNOWN;
  }

  png_set_pHYs(png_ptr, info_ptr, x, y, unit);
  QueryPhysAPIs(png_ptr, info_ptr);

  png_set_pHYs(png_ptr, info_ptr, y, x, PNG_RESOLUTION_METER);
  QueryPhysAPIs(png_ptr, info_ptr);

  png_set_pHYs(png_ptr, info_ptr, 0, 0, PNG_RESOLUTION_UNKNOWN);
  QueryPhysAPIs(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzReadPath(Data, Size);
  FuzzManualInfoPath(Data, Size);
  return 0;
}