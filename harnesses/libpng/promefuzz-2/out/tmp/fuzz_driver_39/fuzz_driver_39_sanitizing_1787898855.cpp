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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static int ReadS32(const uint8_t *data, size_t size, size_t offset) {
  return static_cast<int>(ReadU32(data, size, offset));
}

static void PngErrorFn(png_structp, png_const_charp) {
  std::abort();
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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

  int heuristic_method = ReadS32(Data, Size, 0);
  int num_weights = ReadS32(Data, Size, 4);
  int filter_method = ReadS32(Data, Size, 8);
  int filters = ReadS32(Data, Size, 12);
  int compression_method = ReadS32(Data, Size, 16);
  int compression_level = ReadS32(Data, Size, 20);
  int flush_rows = ReadS32(Data, Size, 24);

  double dweights[4] = {0.0, 0.0, 0.0, 0.0};
  double dcosts[4] = {0.0, 0.0, 0.0, 0.0};
  png_fixed_point fweights[4] = {0, 0, 0, 0};
  png_fixed_point fcosts[4] = {0, 0, 0, 0};

  for (size_t i = 0; i < 4; ++i) {
    uint32_t raw_w = ReadU32(Data, Size, 28 + i * 4);
    uint32_t raw_c = ReadU32(Data, Size, 44 + i * 4);
    dweights[i] = static_cast<double>(raw_w % 1000000) / 100000.0;
    dcosts[i] = static_cast<double>(raw_c % 1000000) / 100000.0;
    fweights[i] = static_cast<png_fixed_point>(ReadS32(Data, Size, 60 + i * 4));
    fcosts[i] = static_cast<png_fixed_point>(ReadS32(Data, Size, 76 + i * 4));
  }

  png_set_filter_heuristics(png_ptr, heuristic_method, num_weights, dweights, dcosts);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(png_ptr, heuristic_method, num_weights, fweights, fcosts);
#endif

  png_set_filter(png_ptr, filter_method, filters);
  png_set_compression_method(png_ptr, compression_method);
  png_set_compression_level(png_ptr, compression_level);
  png_set_flush(png_ptr, flush_rows);

  png_set_filter_heuristics(
      png_ptr,
      heuristic_method ^ PNG_FILTER_HEURISTIC_LAST,
      num_weights < 0 ? -num_weights : num_weights,
      (Size & 1) ? dweights : nullptr,
      (Size & 2) ? dcosts : nullptr);

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(
      png_ptr,
      heuristic_method + 1,
      num_weights ^ 3,
      (Size & 4) ? fweights : nullptr,
      (Size & 8) ? fcosts : nullptr);
#endif

  png_set_filter(png_ptr, 0, filters);
  png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters ^ 0xFF);
  png_set_compression_method(png_ptr, 8);
  png_set_compression_method(png_ptr, compression_method ^ 8);
  png_set_compression_level(png_ptr, compression_level ^ 0x7fffffff);
  png_set_flush(png_ptr, flush_rows < 0 ? 0 : flush_rows);
  png_set_flush(png_ptr, flush_rows ^ 0x7fffffff);

  png_set_filter_heuristics(nullptr, heuristic_method, num_weights, dweights, dcosts);
#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
  png_set_filter_heuristics_fixed(nullptr, heuristic_method, num_weights, fweights, fcosts);
#endif
  png_set_filter(nullptr, filter_method, filters);
  png_set_compression_method(nullptr, compression_method);
  png_set_compression_level(nullptr, compression_level);
  png_set_flush(nullptr, flush_rows);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}