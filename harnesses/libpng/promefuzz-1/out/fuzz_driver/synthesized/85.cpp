// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include <png.h>

extern "C" int LLVMFuzzerTestOneInput_85(const uint8_t *Data, size_t Size) {
  png_byte in4[4] = {0, 0, 0, 0};
  png_byte in2[2] = {0, 0};

  if (Size > 0) {
    size_t copy4 = Size < 4 ? Size : 4;
    memcpy(in4, Data, copy4);

    size_t copy2 = Size < 2 ? Size : 2;
    memcpy(in2, Data, copy2);
  }

  png_uint_32 u32 = png_get_uint_32(in4);
  png_int_32 i32 = png_get_int_32(in4);

  png_byte out4a[4] = {0, 0, 0, 0};
  png_byte out4b[4] = {0, 0, 0, 0};
  png_byte out2[2] = {0, 0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, i32);
  png_save_uint_16(out2, static_cast<unsigned int>((in2[0] << 8) | in2[1]));

  (void)png_get_uint_32(out4a);
  (void)png_get_int_32(out4a);
  (void)png_get_uint_32(out4b);
  (void)png_get_int_32(out4b);

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_int_32 offset_x = i32;
      png_int_32 offset_y = static_cast<png_int_32>(
          (Size >= 8) ? png_get_int_32(Data + 4) : static_cast<png_int_32>(u32 ^ 0x80000000u));
      int unit_type = (Size > 8) ? static_cast<int>(Data[8]) : 0;

      png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);
      png_set_oFFs(png_ptr, info_ptr, -offset_x, -offset_y, unit_type ^ 1);
      png_set_oFFs(png_ptr, info_ptr,
                   static_cast<png_int_32>(png_get_int_32(out4a)),
                   static_cast<png_int_32>(png_get_int_32(out4b)),
                   unit_type & 0xff);

      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    png_destroy_write_struct(&png_ptr, nullptr);
  }

  png_set_oFFs(nullptr, nullptr, i32, i32, 0);

  return 0;
}