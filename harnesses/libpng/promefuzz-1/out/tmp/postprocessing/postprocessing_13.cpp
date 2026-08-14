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
#include <vector>

static png_uint_32 ReadU32LE(const uint8_t *p) {
  return static_cast<png_uint_32>(p[0]) |
         (static_cast<png_uint_32>(p[1]) << 8) |
         (static_cast<png_uint_32>(p[2]) << 16) |
         (static_cast<png_uint_32>(p[3]) << 24);
}

static png_int_32 ReadI32LE(const uint8_t *p) {
  return static_cast<png_int_32>(ReadU32LE(p));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t in4[4] = {0, 0, 0, 0};
  uint8_t in2[2] = {0, 0};

  for (size_t i = 0; i < 4 && i < Size; ++i) in4[i] = Data[i];
  for (size_t i = 0; i < 2 && i < Size; ++i) in2[i] = Data[i];

  png_uint_32 u32 = png_get_uint_32(in4);
  png_int_32 i32 = png_get_int_32(in4);

  uint8_t out4a[4] = {0, 0, 0, 0};
  uint8_t out4b[4] = {0, 0, 0, 0};
  uint8_t out2[2] = {0, 0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, i32);
  png_save_uint_16(out2, static_cast<unsigned int>((in2[0] << 8) | in2[1]));

  (void)png_get_uint_32(out4a);
  (void)png_get_int_32(out4b);

  if (Size >= 8) {
    png_uint_32 u32b = png_get_uint_32(Data + 4);
    png_int_32 i32b = png_get_int_32(Data + 4);
    png_save_uint_32(out4a, u32b ^ u32);
    png_save_int_32(out4b, static_cast<png_int_32>(i32b ^ i32));
    (void)png_get_uint_32(out4a);
    (void)png_get_int_32(out4b);
  }

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_int_32 offset_x = 0;
      png_int_32 offset_y = 0;
      int unit_type = 0;

      if (Size >= 4) offset_x = ReadI32LE(Data);
      if (Size >= 8) offset_y = ReadI32LE(Data + 4);
      if (Size >= 9) unit_type = static_cast<int>(Data[8]);

      png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);
      png_set_oFFs(png_ptr, info_ptr, i32, static_cast<png_int_32>(u32), unit_type & 1);

      if (Size >= 16) {
        png_int_32 ox2 = ReadI32LE(Data + 8);
        png_int_32 oy2 = ReadI32LE(Data + 12);
        png_set_oFFs(png_ptr, info_ptr, ox2, oy2, unit_type ^ 1);
      }
    }
    png_destroy_write_struct(&png_ptr, &info_ptr);
  }

  return 0;
}