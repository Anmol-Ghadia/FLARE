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
#include <cstring>

static png_uint_32 ReadU32(const uint8_t *data) {
  return (static_cast<png_uint_32>(data[0]) << 24) |
         (static_cast<png_uint_32>(data[1]) << 16) |
         (static_cast<png_uint_32>(data[2]) << 8) |
         static_cast<png_uint_32>(data[3]);
}

static png_int_32 ReadS32(const uint8_t *data) {
  return static_cast<png_int_32>(ReadU32(data));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_int_32 ox = 0;
  png_int_32 oy = 0;
  int unit_type = 0;

  png_get_oFFs(png_ptr, info_ptr, &ox, &oy, &unit_type);
  png_get_oFFs(nullptr, info_ptr, &ox, &oy, &unit_type);
  png_get_oFFs(png_ptr, nullptr, &ox, &oy, &unit_type);
  png_get_oFFs(png_ptr, info_ptr, nullptr, &oy, &unit_type);
  png_get_oFFs(png_ptr, info_ptr, &ox, nullptr, &unit_type);
  png_get_oFFs(png_ptr, info_ptr, &ox, &oy, nullptr);

  png_get_x_offset_inches(png_ptr, info_ptr);
  png_get_y_offset_inches(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif
  png_get_y_offset_pixels(png_ptr, info_ptr);
  png_get_y_offset_microns(png_ptr, info_ptr);

#ifdef PNG_pHYs_SUPPORTED
  png_uint_32 res_x = 0, res_y = 0;
  int phys_unit = 0;
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &phys_unit);
  png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &res_y, &phys_unit);
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, nullptr, &phys_unit);
  png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, nullptr);
#endif

  if (Size >= 9) {
    size_t offset = 0;
    while (offset + 9 <= Size) {
      png_int_32 x = ReadS32(Data + offset);
      png_int_32 y = ReadS32(Data + offset + 4);
      int unit = Data[offset + 8] % 4;

      png_set_oFFs(png_ptr, info_ptr, x, y, unit);

      png_int_32 got_x = 0;
      png_int_32 got_y = 0;
      int got_unit = -1;
      png_get_oFFs(png_ptr, info_ptr, &got_x, &got_y, &got_unit);

      png_get_x_offset_inches(png_ptr, info_ptr);
      png_get_y_offset_inches(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
      png_get_x_offset_inches_fixed(png_ptr, info_ptr);
      png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif
      png_get_y_offset_pixels(png_ptr, info_ptr);
      png_get_y_offset_microns(png_ptr, info_ptr);

#ifdef PNG_pHYs_SUPPORTED
      png_uint_32 res_x = 0, res_y = 0;
      int phys_unit = 0;
      png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &phys_unit);
#endif

      offset += 9;
    }
  } else {
    png_set_oFFs(png_ptr, info_ptr, 0, 0, PNG_OFFSET_PIXEL);
    png_get_oFFs(png_ptr, info_ptr, &ox, &oy, &unit_type);
    png_get_x_offset_inches(png_ptr, info_ptr);
    png_get_y_offset_inches(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    png_get_x_offset_inches_fixed(png_ptr, info_ptr);
    png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif
    png_get_y_offset_pixels(png_ptr, info_ptr);
    png_get_y_offset_microns(png_ptr, info_ptr);

    png_set_oFFs(png_ptr, info_ptr, 1000, -1000, PNG_OFFSET_MICROMETER);
    png_get_oFFs(png_ptr, info_ptr, &ox, &oy, &unit_type);
    png_get_x_offset_inches(png_ptr, info_ptr);
    png_get_y_offset_inches(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    png_get_x_offset_inches_fixed(png_ptr, info_ptr);
    png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif
    png_get_y_offset_pixels(png_ptr, info_ptr);
    png_get_y_offset_microns(png_ptr, info_ptr);
  }

  png_set_oFFs(nullptr, info_ptr, 1, 2, PNG_OFFSET_PIXEL);
  png_set_oFFs(png_ptr, nullptr, 1, 2, PNG_OFFSET_PIXEL);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}