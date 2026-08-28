// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
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
#include <cstdlib>
#include <cstring>

#include <png.h>

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         (static_cast<uint32_t>(data[offset + 3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  int level = static_cast<int>(ReadU32(Data, Size, 0));
  int window_bits = static_cast<int>(ReadU32(Data, Size, 4));
  int flush_rows = static_cast<int>(ReadU32(Data, Size, 8));
  int strategy = static_cast<int>(ReadU32(Data, Size, 12));
  int mem_level = static_cast<int>(ReadU32(Data, Size, 16));
  int heuristic_method = static_cast<int>(ReadU32(Data, Size, 20));
  int num_weights = static_cast<int>(ReadU32(Data, Size, 24));

  png_fixed_point weights[4] = {0, 0, 0, 0};
  png_fixed_point costs[4] = {0, 0, 0, 0};

  for (int i = 0; i < 4; ++i) {
    size_t off = 28 + static_cast<size_t>(i) * 4;
    weights[i] = static_cast<png_fixed_point>(ReadU32(Data, Size, off));
  }
  for (int i = 0; i < 4; ++i) {
    size_t off = 44 + static_cast<size_t>(i) * 4;
    costs[i] = static_cast<png_fixed_point>(ReadU32(Data, Size, off));
  }

  png_set_compression_level(png_ptr, level);
  png_set_compression_window_bits(png_ptr, window_bits);
  png_set_flush(png_ptr, flush_rows);
  png_set_compression_strategy(png_ptr, strategy);
  png_set_compression_mem_level(png_ptr, mem_level);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(
      png_ptr,
      heuristic_method,
      num_weights,
      weights,
      costs);
#endif

  png_set_compression_level(png_ptr, -1);
  png_set_compression_level(png_ptr, 0);
  png_set_compression_level(png_ptr, 9);
  png_set_compression_window_bits(png_ptr, 0);
  png_set_compression_window_bits(png_ptr, 8);
  png_set_compression_window_bits(png_ptr, 15);
  png_set_compression_window_bits(png_ptr, 100);
  png_set_flush(png_ptr, -100);
  png_set_flush(png_ptr, 0);
  png_set_flush(png_ptr, 1);
  png_set_compression_strategy(png_ptr, -1);
  png_set_compression_strategy(png_ptr, 0);
  png_set_compression_mem_level(png_ptr, -1);
  png_set_compression_mem_level(png_ptr, 9);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(
      png_ptr,
      PNG_FILTER_HEURISTIC_DEFAULT,
      0,
      nullptr,
      nullptr);
  png_set_filter_heuristics_fixed(
      png_ptr,
      PNG_FILTER_HEURISTIC_WEIGHTED,
      4,
      weights,
      costs);
#endif

  png_set_compression_level(nullptr, level);
  png_set_compression_window_bits(nullptr, window_bits);
  png_set_flush(nullptr, flush_rows);
  png_set_compression_strategy(nullptr, strategy);
  png_set_compression_mem_level(nullptr, mem_level);
#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(nullptr, heuristic_method, num_weights, weights, costs);
#endif

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}