// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <cstdio>
#include <cstring>
#include <vector>

#include <png.h>

static png_uint_32 ReadU32(const uint8_t* data) {
  return static_cast<png_uint_32>(data[0]) |
         (static_cast<png_uint_32>(data[1]) << 8) |
         (static_cast<png_uint_32>(data[2]) << 16) |
         (static_cast<png_uint_32>(data[3]) << 24);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
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

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_chunk_cache_max(nullptr, 0);
  (void)png_get_chunk_malloc_max(nullptr);
  (void)png_get_user_width_max(nullptr);
  (void)png_get_user_height_max(nullptr);
  png_set_user_limits(nullptr, 0, 0);
  (void)png_get_chunk_cache_max(nullptr);

  png_uint_32 a = 0;
  png_uint_32 b = 0;
  png_uint_32 c = 0;
  if (Size >= 4) a = ReadU32(Data);
  if (Size >= 8) b = ReadU32(Data + 4);
  if (Size >= 12) c = ReadU32(Data + 8);

  png_set_chunk_cache_max(png_ptr, a);
  (void)png_get_chunk_cache_max(png_ptr);
  (void)png_get_chunk_malloc_max(png_ptr);
  (void)png_get_user_width_max(png_ptr);
  (void)png_get_user_height_max(png_ptr);

  png_set_user_limits(png_ptr, b, c);
  (void)png_get_user_width_max(png_ptr);
  (void)png_get_user_height_max(png_ptr);
  (void)png_get_chunk_cache_max(png_ptr);
  (void)png_get_chunk_malloc_max(png_ptr);

  png_set_user_limits(png_ptr, 0, 0);
  png_set_user_limits(png_ptr, 0x7fffffffU, 0x7fffffffU);
  png_set_chunk_cache_max(png_ptr, 0);
  png_set_chunk_cache_max(png_ptr, 1);
  png_set_chunk_cache_max(png_ptr, 0xFFFFFFFFU);

  if (Size > 0) {
    size_t offset = 0;
    while (offset + 8 <= Size) {
      png_uint_32 v1 = ReadU32(Data + offset);
      png_uint_32 v2 = ReadU32(Data + offset + 4);
      png_set_user_limits(png_ptr, v1, v2);
      png_set_chunk_cache_max(png_ptr, v1 ^ v2);
      (void)png_get_user_width_max(png_ptr);
      (void)png_get_user_height_max(png_ptr);
      (void)png_get_chunk_cache_max(png_ptr);
      (void)png_get_chunk_malloc_max(png_ptr);
      offset += 8;
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}