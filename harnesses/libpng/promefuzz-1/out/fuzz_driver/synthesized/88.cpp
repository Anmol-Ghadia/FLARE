// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
#include <cstdint>
#include <cstddef>
#include <png.h>

static png_uint_32 ReadU32BE(const uint8_t* p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         static_cast<png_uint_32>(p[3]);
}

static png_int_32 ReadI32BE(const uint8_t* p) {
  return static_cast<png_int_32>(ReadU32BE(p));
}

extern "C" int LLVMFuzzerTestOneInput_88(const uint8_t *Data, size_t Size) {
  uint8_t in4[4] = {0, 0, 0, 0};

  for (size_t i = 0; i < 4 && i < Size; ++i) {
    in4[i] = Data[i];
  }

  png_uint_32 u32 = png_get_uint_32(in4);
  png_int_32 i32 = png_get_int_32(in4);

  uint8_t out4a[4] = {0, 0, 0, 0};
  uint8_t out4b[4] = {0, 0, 0, 0};
  uint8_t out2[2] = {0, 0};

  png_save_uint_32(out4a, u32);
  png_save_int_32(out4b, i32);
  png_save_uint_16(out2, static_cast<unsigned int>(ReadU32BE(in4)));

  (void)png_get_uint_32(out4a);
  (void)png_get_int_32(out4a);
  (void)png_get_uint_32(out4b);
  (void)png_get_int_32(out4b);

  if (Size >= 8) {
    png_uint_32 u32b = ReadU32BE(Data + 4);
    png_int_32 i32b = ReadI32BE(Data + 4);
    uint8_t tmp4[4] = {0, 0, 0, 0};

    png_save_uint_32(tmp4, u32 ^ u32b);
    (void)png_get_uint_32(tmp4);

    png_save_int_32(tmp4, static_cast<png_int_32>(i32 ^ i32b));
    (void)png_get_int_32(tmp4);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_int_32 offset_x = i32;
      png_int_32 offset_y = (Size >= 8) ? ReadI32BE(Data + 4)
                                        : static_cast<png_int_32>(~u32);
      int unit_type = (Size > 8) ? static_cast<int>(Data[8]) : 0;

      png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);
      png_set_oFFs(png_ptr, info_ptr, -offset_x, -offset_y, unit_type ^ 1);
      png_set_oFFs(png_ptr, info_ptr, 0, 0, unit_type);

      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    png_destroy_write_struct(&png_ptr, nullptr);
  }

  return 0;
}