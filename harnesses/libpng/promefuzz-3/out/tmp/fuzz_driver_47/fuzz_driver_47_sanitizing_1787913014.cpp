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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static png_structp CreatePngStruct() {
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  }
  return png_ptr;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr = CreatePngStruct();
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (Size >= 16) {
    png_int_32 x_off = 0;
    png_int_32 y_off = 0;
    memcpy(&x_off, Data, sizeof(x_off));
    memcpy(&y_off, Data + 4, sizeof(y_off));

    png_uint_32 res_x = 0;
    png_uint_32 res_y = 0;
    memcpy(&res_x, Data + 8, sizeof(res_x));
    memcpy(&res_y, Data + 12, sizeof(res_y));

    int unit_offs = (Size > 16) ? (Data[16] % 4) : PNG_OFFSET_PIXEL;
    int unit_phys = (Size > 17) ? (Data[17] % 4) : PNG_RESOLUTION_UNKNOWN;

    png_set_oFFs(png_ptr, info_ptr, x_off, y_off, unit_offs);
    png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_phys);

    (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
    png_uint_32 out_x = 0, out_y = 0;
    int out_unit = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x, &out_y, &out_unit);
#endif

    int mask = 0;
    if (Size > 18) {
      mask = Data[18];
      if (Size > 19) {
        mask |= (int)Data[19] << 8;
      }
      if (Size > 20) {
        mask |= (int)Data[20] << 16;
      }
      if (Size > 21) {
        mask |= (int)Data[21] << 24;
      }
    } else {
      mask = PNG_INFO_oFFs | PNG_INFO_pHYs;
    }

    png_set_invalid(png_ptr, info_ptr, mask);

    (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
    png_uint_32 out_x2 = 0, out_y2 = 0;
    int out_unit2 = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x2, &out_y2, &out_unit2);
#endif

    png_set_oFFs(png_ptr, info_ptr, x_off ^ 0x7fffffff, y_off ^ 0x7fffffff,
                 (unit_offs + 1) % 4);
    png_set_pHYs(png_ptr, info_ptr, ~res_x, ~res_y, (unit_phys + 1) % 4);

    (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
    png_uint_32 out_x3 = 0, out_y3 = 0;
    int out_unit3 = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x3, &out_y3, &out_unit3);
#endif
  } else {
    png_set_oFFs(png_ptr, info_ptr, 0, 0, PNG_OFFSET_PIXEL);
    png_set_pHYs(png_ptr, info_ptr, 0, 0, PNG_RESOLUTION_UNKNOWN);
    (void)png_get_x_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
    png_uint_32 out_x = 0, out_y = 0;
    int out_unit = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &out_x, &out_y, &out_unit);
#endif
  }

  png_info *raw_info = (png_info *)malloc(sizeof(png_info));
  if (raw_info != nullptr) {
    memset(raw_info, 0xAB, sizeof(png_info));
    png_infop tmp = raw_info;
    png_info_init_3(&tmp, sizeof(png_info));
    if (tmp != nullptr) {
      png_set_oFFs(png_ptr, tmp, 1, -1, PNG_OFFSET_MICROMETER);
      png_set_pHYs(png_ptr, tmp, 1, 1, PNG_RESOLUTION_METER);
      (void)png_get_x_offset_microns(png_ptr, tmp);
#ifdef PNG_FIXED_POINT_SUPPORTED
      (void)png_get_x_offset_inches_fixed(png_ptr, tmp);
#endif
      png_set_invalid(png_ptr, tmp, PNG_INFO_oFFs | PNG_INFO_pHYs);
      free(tmp);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}