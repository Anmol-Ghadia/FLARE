// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_filter_heuristics_fixed at pngwrite.c:1138:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
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
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

static int ReadI32(const uint8_t *data, size_t size, size_t offset) {
  return static_cast<int>(ReadU32(data, size, offset));
}

extern "C" int LLVMFuzzerTestOneInput_36(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png_ptr != NULL) {
    int strategy1 = ReadI32(Data, Size, 0);
    int method1 = ReadI32(Data, Size, 4);
    int heuristic_method = ReadI32(Data, Size, 8);
    int num_weights = ReadI32(Data, Size, 12);
    int level1 = ReadI32(Data, Size, 16);
    int text_strategy1 = ReadI32(Data, Size, 20);
    int flush1 = ReadI32(Data, Size, 24);

    png_fixed_point weights[4] = {0, 0, 0, 0};
    png_fixed_point costs[4] = {0, 0, 0, 0};

    for (size_t i = 0; i < 4; ++i) {
      weights[i] = static_cast<png_fixed_point>(ReadI32(Data, Size, 28 + i * 4));
      costs[i] = static_cast<png_fixed_point>(ReadI32(Data, Size, 44 + i * 4));
    }

    png_set_compression_strategy(png_ptr, strategy1);
    png_set_compression_method(png_ptr, method1);
#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
    png_set_filter_heuristics_fixed(png_ptr, heuristic_method, num_weights,
                                    weights, costs);
#endif
    png_set_compression_level(png_ptr, level1);
    png_set_text_compression_strategy(png_ptr, text_strategy1);
    png_set_flush(png_ptr, flush1);

    if (Size > 60) {
      int strategy2 = ReadI32(Data, Size, 60);
      int method2 = ReadI32(Data, Size, 64);
      int level2 = ReadI32(Data, Size, 68);
      int text_strategy2 = ReadI32(Data, Size, 72);
      int flush2 = ReadI32(Data, Size, 76);

      png_set_compression_strategy(png_ptr, strategy2);
      png_set_compression_method(png_ptr, method2);
      png_set_compression_level(png_ptr, level2);
      png_set_text_compression_strategy(png_ptr, text_strategy2);
      png_set_flush(png_ptr, flush2);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
      int heuristic_method2 = (Size > 80) ? ReadI32(Data, Size, 80) : 0;
      int num_weights2 = (Size > 84) ? ReadI32(Data, Size, 84) : 0;
      png_set_filter_heuristics_fixed(png_ptr, heuristic_method2, num_weights2,
                                      costs, weights);
#endif
    }

    png_destroy_write_struct(&png_ptr, NULL);
  }

  png_set_compression_strategy(NULL, ReadI32(Data, Size, 0));
  png_set_compression_method(NULL, ReadI32(Data, Size, 4));
#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(NULL, ReadI32(Data, Size, 8),
                                  ReadI32(Data, Size, 12), NULL, NULL);
#endif
  png_set_compression_level(NULL, ReadI32(Data, Size, 16));
  png_set_text_compression_strategy(NULL, ReadI32(Data, Size, 20));
  png_set_flush(NULL, ReadI32(Data, Size, 24));

  return 0;
}