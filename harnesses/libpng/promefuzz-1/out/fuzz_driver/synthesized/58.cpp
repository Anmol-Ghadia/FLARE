// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include <png.h>

static png_uint_32 ReadU32LE(const uint8_t* data) {
  if (data == nullptr) return 0;
  return static_cast<png_uint_32>(data[0]) |
         (static_cast<png_uint_32>(data[1]) << 8) |
         (static_cast<png_uint_32>(data[2]) << 16) |
         (static_cast<png_uint_32>(data[3]) << 24);
}

static png_int_32 ToI32(png_uint_32 v) {
  return static_cast<png_int_32>(v);
}

extern "C" int LLVMFuzzerTestOneInput_58(const uint8_t *Data, size_t Size) {
  uint8_t local4[4] = {0, 0, 0, 0};
  uint8_t local4b[4] = {0, 0, 0, 0};
  uint8_t local2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(local4, Data, 4);
  } else if (Size > 0) {
    memcpy(local4, Data, Size);
  }

  png_uint_32 u32 = png_get_uint_32(local4);
  png_int_32 i32 = png_get_int_32(local4);

  png_save_uint_32(local4b, u32);
  png_save_int_32(local4, i32);
  png_save_uint_16(local2, static_cast<unsigned int>(u32));

  (void)png_get_uint_32(local4b);
  (void)png_get_int_32(local4b);

  png_uint_32 derived = ReadU32LE(local4);
  png_save_uint_32(local4b, derived);
  png_save_int_32(local4, ToI32(derived ^ u32));
  png_save_uint_16(local2, static_cast<unsigned int>(derived >> 16));

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_int_32 offset_x = i32;
      png_int_32 offset_y = ToI32(derived);

      int unit_type = 0;
      if (Size > 4) {
        unit_type = static_cast<int>(Data[4]);
      }

      png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);
      png_set_oFFs(png_ptr, info_ptr, -offset_x, -offset_y, unit_type ^ 1);
      png_set_oFFs(png_ptr, info_ptr,
                   static_cast<png_int_32>(0),
                   static_cast<png_int_32>(0),
                   unit_type & 1);

      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    png_destroy_write_struct(&png_ptr, nullptr);
  }

  png_set_oFFs(nullptr, nullptr, i32, ToI32(u32), 0);

  return 0;
}