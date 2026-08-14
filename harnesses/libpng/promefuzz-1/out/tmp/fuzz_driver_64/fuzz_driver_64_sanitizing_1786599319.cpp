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
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

static png_uint_32 ReadU32(const uint8_t *data) {
  return png_get_uint_32(reinterpret_cast<png_const_bytep>(data));
}

static png_int_32 ReadI32(const uint8_t *data) {
  return png_get_int_32(reinterpret_cast<png_const_bytep>(data));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t in4[4] = {0, 0, 0, 0};
  uint8_t in4b[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t in2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(in4, Data, 4);
  } else if (Size > 0) {
    memcpy(in4, Data, Size);
  }

  if (Size >= 8) {
    memcpy(in4b, Data + 4, 4);
  } else if (Size > 4) {
    memcpy(in4b, Data + 4, Size - 4);
  }

  if (Size >= 2) {
    memcpy(in2, Data, 2);
  } else if (Size > 0) {
    memcpy(in2, Data, Size);
  }

  png_uint_32 u1 = ReadU32(in4);
  png_uint_32 u2 = ReadU32(in4b);
  png_int_32 i1 = ReadI32(in4);
  png_int_32 i2 = ReadI32(in4b);

  uint8_t out4a[4] = {0};
  uint8_t out4b[4] = {0};
  uint8_t out4c[4] = {0};
  uint8_t out2a[2] = {0};
  uint8_t out2b[2] = {0};

  png_save_uint_32(reinterpret_cast<png_bytep>(out4a), u1);
  png_save_uint_32(reinterpret_cast<png_bytep>(out4b), static_cast<png_uint_32>(i1));
  png_save_int_32(reinterpret_cast<png_bytep>(out4c), i2);
  png_save_uint_16(reinterpret_cast<png_bytep>(out2a),
                   static_cast<unsigned int>((Size > 0) ? Data[0] : 0));
  png_save_uint_16(reinterpret_cast<png_bytep>(out2b),
                   static_cast<unsigned int>(u2));

  (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(out4a));
  (void)png_get_uint_32(reinterpret_cast<png_const_bytep>(out4b));
  (void)png_get_int_32(reinterpret_cast<png_const_bytep>(out4c));

  png_structp png_ptr = png_create_info_struct(nullptr);
  png_infop info_ptr = png_create_info_struct(nullptr);

  if (png_ptr != nullptr && info_ptr != nullptr) {
    int unit_type = 0;
    if (Size > 8) {
      unit_type = Data[8] % 4;
    }

    png_set_oFFs(png_ptr, info_ptr, i1, i2, unit_type);
    png_set_oFFs(png_ptr, info_ptr, static_cast<png_int_32>(u1),
                 static_cast<png_int_32>(u2), unit_type ^ 1);
    png_set_oFFs(png_ptr, info_ptr, -i1, -i2, 0);
  }

  png_set_oFFs(nullptr, info_ptr, i1, i2, 0);
  png_set_oFFs(png_ptr, nullptr, i1, i2, 1);
  png_set_oFFs(nullptr, nullptr, i1, i2, 2);

  if (info_ptr != nullptr) {
    png_destroy_info_struct(png_ptr, &info_ptr);
  }
  if (png_ptr != nullptr) {
    png_destroy_info_struct(nullptr, &png_ptr);
  }

  return 0;
}