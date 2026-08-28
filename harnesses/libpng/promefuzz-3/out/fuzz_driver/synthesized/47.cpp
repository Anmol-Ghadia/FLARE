// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_invalid at pngset.c:1626:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_info_init_3 at png.c:423:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_inches_fixed at pngget.c:379:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static png_structp CreatePngStruct() {
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  }
  return png_ptr;
}

static uint32_t ReadU32(const uint8_t *Data, size_t Size, size_t Off) {
  uint32_t v = 0;
  if (Off < Size) {
    size_t n = Size - Off;
    if (n > sizeof(v)) n = sizeof(v);
    memcpy(&v, Data + Off, n);
  }
  return v;
}

static int32_t ReadS32(const uint8_t *Data, size_t Size, size_t Off) {
  int32_t v = 0;
  if (Off < Size) {
    size_t n = Size - Off;
    if (n > sizeof(v)) n = sizeof(v);
    memcpy(&v, Data + Off, n);
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_47(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = CreatePngStruct();
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    if (png_get_valid(png_ptr, info_ptr, 0) || 1) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
    }
    return 0;
  }

  int mode = Size > 0 ? (Data[0] % 4) : 0;
  png_int_32 x_off = ReadS32(Data, Size, 1);
  png_int_32 y_off = ReadS32(Data, Size, 5);
  png_uint_32 res_x = ReadU32(Data, Size, 9);
  png_uint_32 res_y = ReadU32(Data, Size, 13);
  int unit_offs = Size > 17 ? (Data[17] % 4) : PNG_OFFSET_PIXEL;
  int unit_phys = Size > 18 ? (Data[18] % 4) : PNG_RESOLUTION_UNKNOWN;
  int mask = (int)ReadU32(Data, Size, 19);

  switch (mode) {
    case 0:
      png_set_oFFs(png_ptr, info_ptr, x_off, y_off, unit_offs);
      png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_phys);
      break;
    case 1:
      png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_phys);
      png_set_oFFs(png_ptr, info_ptr, x_off, y_off, unit_offs);
      break;
    case 2:
      png_set_oFFs(png_ptr, info_ptr, 0, 0, PNG_OFFSET_MICROMETER);
      png_set_pHYs(png_ptr, info_ptr, 1, 1, PNG_RESOLUTION_METER);
      png_set_oFFs(png_ptr, info_ptr, x_off, y_off, unit_offs);
      png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_phys);
      break;
    default:
      png_set_oFFs(png_ptr, info_ptr, x_off ^ 0x7fffffff, y_off ^ 0x7fffffff,
                   unit_offs);
      png_set_pHYs(png_ptr, info_ptr, ~res_x, ~res_y, unit_phys);
      break;
  }

  (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
  {
    png_uint_32 out_x = 0, out_y = 0;
    int out_unit = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x, &out_y, &out_unit);
  }
#endif

  png_set_invalid(png_ptr, info_ptr, mask);
  png_set_invalid(png_ptr, info_ptr, PNG_INFO_oFFs);
  png_set_invalid(png_ptr, info_ptr, PNG_INFO_pHYs);

  (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
  {
    png_uint_32 out_x = 0, out_y = 0;
    int out_unit = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x, &out_y, &out_unit);
  }
#endif

  png_infop info2 = NULL;
  png_info_init_3(&info2, 0);
  if (info2 != NULL) {
    png_set_oFFs(png_ptr, info2, x_off, y_off, PNG_OFFSET_MICROMETER);
    png_set_pHYs(png_ptr, info2, res_x, res_y, PNG_RESOLUTION_METER);
    (void)png_get_x_offset_microns(png_ptr, info2);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info2);
#endif
#ifdef PNG_pHYs_SUPPORTED
    {
      png_uint_32 out_x = 0, out_y = 0;
      int out_unit = 0;
      (void)png_get_pHYs_dpi(png_ptr, info2, &out_x, &out_y, &out_unit);
    }
#endif
    png_set_invalid(png_ptr, info2, PNG_INFO_oFFs | PNG_INFO_pHYs);
  }

  png_destroy_info_struct(png_ptr, &info2);
  png_destroy_info_struct(png_ptr, &info_ptr);

  if (png_ptr != NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
  }

  return 0;
}