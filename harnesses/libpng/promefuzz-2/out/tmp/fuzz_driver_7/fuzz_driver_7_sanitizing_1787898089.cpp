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
#include <vector>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         (static_cast<uint32_t>(data[offset + 3]));
}

static void WriteDummyFile(const uint8_t *data, size_t size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size > 0) fwrite(data, 1, size, fp);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  (void)png_get_chunk_cache_max(NULL);
  (void)png_get_user_width_max(NULL);
  (void)png_get_user_height_max(NULL);
  (void)png_get_chunk_malloc_max(NULL);
  png_set_user_limits(NULL, 0, 0);
  png_set_chunk_cache_max(NULL, 0);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  uint32_t a = ReadU32(Data, Size, 0);
  uint32_t b = ReadU32(Data, Size, 4);
  uint32_t c = ReadU32(Data, Size, 8);
  uint32_t d = ReadU32(Data, Size, 12);

  png_uint_32 width_limit1 = static_cast<png_uint_32>(a);
  png_uint_32 height_limit1 = static_cast<png_uint_32>(b);
  png_uint_32 cache_limit1 = static_cast<png_uint_32>(c);

  png_set_user_limits(png_ptr, width_limit1, height_limit1);
  png_set_chunk_cache_max(png_ptr, cache_limit1);

  volatile png_uint_32 gw1 = png_get_user_width_max(png_ptr);
  volatile png_uint_32 gh1 = png_get_user_height_max(png_ptr);
  volatile png_uint_32 gc1 = png_get_chunk_cache_max(png_ptr);
  volatile png_alloc_size_t gm1 = png_get_chunk_malloc_max(png_ptr);
  (void)gw1;
  (void)gh1;
  (void)gc1;
  (void)gm1;

  png_uint_32 width_limit2 =
      (Size > 16 && (Data[16] & 1)) ? 0x7fffffffU : static_cast<png_uint_32>(d);
  png_uint_32 height_limit2 =
      (Size > 17 && (Data[17] & 1)) ? 0x7fffffffU : static_cast<png_uint_32>(~a);
  png_uint_32 cache_limit2 =
      (Size > 18) ? static_cast<png_uint_32>(Data[18]) : 0;

  png_set_user_limits(png_ptr, width_limit2, height_limit2);
  png_set_chunk_cache_max(png_ptr, cache_limit2);

  volatile png_uint_32 gw2 = png_get_user_width_max(png_ptr);
  volatile png_uint_32 gh2 = png_get_user_height_max(png_ptr);
  volatile png_uint_32 gc2 = png_get_chunk_cache_max(png_ptr);
  volatile png_alloc_size_t gm2 = png_get_chunk_malloc_max(png_ptr);
  (void)gw2;
  (void)gh2;
  (void)gc2;
  (void)gm2;

  if (Size > 19) {
    size_t idx = 19;
    uint8_t rounds = Data[idx++] % 8;
    for (uint8_t i = 0; i < rounds; ++i) {
      png_uint_32 w = 0;
      png_uint_32 h = 0;
      png_uint_32 cc = 0;

      if (idx + 12 <= Size) {
        w = ReadU32(Data, Size, idx);
        h = ReadU32(Data, Size, idx + 4);
        cc = ReadU32(Data, Size, idx + 8);
        idx += 12;
      } else {
        w = static_cast<png_uint_32>(i);
        h = static_cast<png_uint_32>(~i);
        cc = static_cast<png_uint_32>(i * 3);
      }

      if (i & 1) w = 0x7fffffffU;
      if (i & 2) h = 0x7fffffffU;

      png_set_user_limits(png_ptr, w, h);
      png_set_chunk_cache_max(png_ptr, cc);

      volatile png_uint_32 gw = png_get_user_width_max(png_ptr);
      volatile png_uint_32 gh = png_get_user_height_max(png_ptr);
      volatile png_uint_32 gc = png_get_chunk_cache_max(png_ptr);
      volatile png_alloc_size_t gm = png_get_chunk_malloc_max(png_ptr);
      (void)gw;
      (void)gh;
      (void)gc;
      (void)gm;
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}