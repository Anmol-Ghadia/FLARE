// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
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
#include <stdlib.h>
#include <string.h>

static void png_noop_error(png_structp, png_const_charp) {}
static void png_noop_warning(png_structp, png_const_charp) {}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t &offset) {
  if (offset + 4 > size) return 0;
  uint32_t v = (uint32_t)data[offset] |
               ((uint32_t)data[offset + 1] << 8) |
               ((uint32_t)data[offset + 2] << 16) |
               ((uint32_t)data[offset + 3] << 24);
  offset += 4;
  return v;
}

static int read_i32(const uint8_t *data, size_t size, size_t &offset) {
  return (int)read_u32(data, size, offset);
}

extern "C" int LLVMFuzzerTestOneInput_15(const uint8_t *Data, size_t Size) {
  size_t offset = 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                              png_noop_error, png_noop_warning);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  int values[24];
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    values[i] = read_i32(Data, Size, offset);

  png_fixed_point weights[8];
  png_fixed_point costs[8];
  for (size_t i = 0; i < 8; ++i)
    weights[i] = (png_fixed_point)read_i32(Data, Size, offset);
  for (size_t i = 0; i < 8; ++i)
    costs[i] = (png_fixed_point)read_i32(Data, Size, offset);

  png_set_compression_window_bits(png_ptr, values[0]);
  png_set_compression_window_bits(png_ptr, values[1]);
  png_set_compression_window_bits(nullptr, values[2]);

  png_set_text_compression_window_bits(png_ptr, values[3]);
  png_set_text_compression_window_bits(png_ptr, values[4]);
  png_set_text_compression_window_bits(nullptr, values[5]);

  png_set_text_compression_method(png_ptr, values[6]);
  png_set_text_compression_method(png_ptr, values[7]);
  png_set_text_compression_method(nullptr, values[8]);

  png_set_filter_heuristics_fixed(
      png_ptr,
      values[9],
      values[10] & 7,
      weights,
      costs);

  png_set_filter_heuristics_fixed(
      png_ptr,
      values[11],
      0,
      nullptr,
      nullptr);

  png_set_filter_heuristics_fixed(
      nullptr,
      values[12],
      values[13] & 7,
      weights,
      costs);

  png_set_text_compression_mem_level(png_ptr, values[14]);
  png_set_text_compression_mem_level(png_ptr, values[15]);
  png_set_text_compression_mem_level(nullptr, values[16]);

  png_set_text_compression_strategy(png_ptr, values[17]);
  png_set_text_compression_strategy(png_ptr, values[18]);
  png_set_text_compression_strategy(nullptr, values[19]);

#ifdef PNG_WRITE_CUSTOMIZE_COMPRESSION_SUPPORTED
  png_set_compression_level(png_ptr, values[20]);
  png_set_compression_level(png_ptr, values[21]);
  png_set_compression_level(nullptr, values[22]);
#endif

  if (offset < Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data + offset, 1, Size - offset, fp);
      fclose(fp);
    }
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}