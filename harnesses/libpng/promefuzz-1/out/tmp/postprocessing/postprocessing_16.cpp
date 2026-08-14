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
#include <string.h>
#include <setjmp.h>

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

static png_fixed_point ReadFixed(const uint8_t *data, size_t size, size_t offset) {
  return static_cast<png_fixed_point>(ReadU32(data, size, offset));
}

static void WriteDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  int srgb_intent = 0;
#ifdef PNG_sRGB_SUPPORTED
  if (Size > 0) srgb_intent = Data[0] % 8;
  png_set_sRGB(png_ptr, info_ptr, srgb_intent);
#endif

#ifdef PNG_gAMA_SUPPORTED
  png_fixed_point gamma1 = ReadFixed(Data, Size, 1);
  png_fixed_point gamma2 = ReadFixed(Data, Size, 5);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma1);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma2);
#endif

#ifdef PNG_cHRM_SUPPORTED
  png_fixed_point wx = ReadFixed(Data, Size, 9);
  png_fixed_point wy = ReadFixed(Data, Size, 13);
  png_fixed_point rx = ReadFixed(Data, Size, 17);
  png_fixed_point ry = ReadFixed(Data, Size, 21);
  png_fixed_point gx = ReadFixed(Data, Size, 25);
  png_fixed_point gy = ReadFixed(Data, Size, 29);
  png_fixed_point bx = ReadFixed(Data, Size, 33);
  png_fixed_point by = ReadFixed(Data, Size, 37);

  png_fixed_point rX = ReadFixed(Data, Size, 41);
  png_fixed_point rY = ReadFixed(Data, Size, 45);
  png_fixed_point rZ = ReadFixed(Data, Size, 49);
  png_fixed_point gX = ReadFixed(Data, Size, 53);
  png_fixed_point gY = ReadFixed(Data, Size, 57);
  png_fixed_point gZ = ReadFixed(Data, Size, 61);
  png_fixed_point bX = ReadFixed(Data, Size, 65);
  png_fixed_point bY = ReadFixed(Data, Size, 69);
  png_fixed_point bZ = ReadFixed(Data, Size, 73);

  png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
  png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, rX, rY, rZ, gX, gY, gZ, bX, bY, bZ);
#endif

  png_uint_32 flags_to_check[] = {
#ifdef PNG_INFO_gAMA
      PNG_INFO_gAMA,
#endif
#ifdef PNG_INFO_cHRM
      PNG_INFO_cHRM,
#endif
#ifdef PNG_INFO_sRGB
      PNG_INFO_sRGB,
#endif
      0u, 0xffffffffu
  };

  for (size_t i = 0; i < sizeof(flags_to_check) / sizeof(flags_to_check[0]); ++i) {
    (void)png_get_valid(png_ptr, info_ptr, flags_to_check[i]);
  }

#ifdef PNG_gAMA_SUPPORTED
  png_fixed_point out_gamma = 0;
  (void)png_get_gAMA_fixed(png_ptr, info_ptr, &out_gamma);
  (void)png_get_gAMA_fixed(png_ptr, info_ptr, nullptr);
#endif

#ifdef PNG_cHRM_SUPPORTED
  png_fixed_point white_x = 0, white_y = 0, red_x = 0, red_y = 0;
  png_fixed_point green_x = 0, green_y = 0, blue_x = 0, blue_y = 0;
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, &white_x, &white_y, &red_x, &red_y,
                           &green_x, &green_y, &blue_x, &blue_y);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, nullptr, nullptr, nullptr, nullptr,
                           nullptr, nullptr, nullptr, nullptr);

  png_fixed_point out_rX = 0, out_rY = 0, out_rZ = 0;
  png_fixed_point out_gX = 0, out_gY = 0, out_gZ = 0;
  png_fixed_point out_bX = 0, out_bY = 0, out_bZ = 0;
  (void)png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, &out_rX, &out_rY, &out_rZ,
                               &out_gX, &out_gY, &out_gZ,
                               &out_bX, &out_bY, &out_bZ);
  (void)png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, nullptr, nullptr, nullptr,
                               nullptr, nullptr, nullptr,
                               nullptr, nullptr, nullptr);
#endif

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}