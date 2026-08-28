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

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return ((png_uint_32)data[offset] << 24) |
         ((png_uint_32)data[offset + 1] << 16) |
         ((png_uint_32)data[offset + 2] << 8) |
         ((png_uint_32)data[offset + 3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    png_get_chunk_cache_max(nullptr);
    png_get_user_width_max(nullptr);
    png_get_user_height_max(nullptr);
    png_permit_mng_features(nullptr, 0);
    png_set_user_limits(nullptr, 0, 0);
    png_set_chunk_cache_max(nullptr, 0);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_get_chunk_cache_max(png_ptr);
    png_get_user_width_max(png_ptr);
    png_get_user_height_max(png_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_uint_32 width1 = ReadU32(Data, Size, 0);
  png_uint_32 height1 = ReadU32(Data, Size, 4);
  png_uint_32 cache1 = ReadU32(Data, Size, 8);
  png_uint_32 mng1 = ReadU32(Data, Size, 12);

  png_set_user_limits(png_ptr, width1, height1);
  png_set_chunk_cache_max(png_ptr, cache1);
  png_uint_32 permitted1 = png_permit_mng_features(png_ptr, mng1);

  volatile png_uint_32 r1 = png_get_user_width_max(png_ptr);
  volatile png_uint_32 r2 = png_get_user_height_max(png_ptr);
  volatile png_uint_32 r3 = png_get_chunk_cache_max(png_ptr);
  volatile png_uint_32 r4 = png_permit_mng_features(png_ptr, permitted1 ^ 0xffffffffu);
  (void)r1;
  (void)r2;
  (void)r3;
  (void)r4;

  png_uint_32 width2 = (Size > 16) ? ReadU32(Data, Size, 16) : 0x7fffffffu;
  png_uint_32 height2 = (Size > 20) ? ReadU32(Data, Size, 20) : 0x7fffffffu;
  png_uint_32 cache2 = (Size > 24) ? ReadU32(Data, Size, 24) : 0;
  png_uint_32 mng2 = (Size > 28) ? ReadU32(Data, Size, 28) : PNG_ALL_MNG_FEATURES;

  png_set_user_limits(png_ptr, width2, height2);
  png_set_chunk_cache_max(png_ptr, cache2);
  volatile png_uint_32 permitted2 = png_permit_mng_features(png_ptr, mng2);
  volatile png_uint_32 r5 = png_get_user_width_max(png_ptr);
  volatile png_uint_32 r6 = png_get_user_height_max(png_ptr);
  volatile png_uint_32 r7 = png_get_chunk_cache_max(png_ptr);
  (void)permitted2;
  (void)r5;
  (void)r6;
  (void)r7;

  png_get_chunk_cache_max(nullptr);
  png_get_user_width_max(nullptr);
  png_get_user_height_max(nullptr);
  png_permit_mng_features(nullptr, mng1);
  png_set_user_limits(nullptr, width1, height1);
  png_set_chunk_cache_max(nullptr, cache1);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}