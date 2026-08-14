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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint32_t seed = 0;
  for (size_t i = 0; i < Size && i < 4; ++i) {
    seed = (seed << 8) | Data[i];
  }

  png_uint_32 width = (Size > 4 ? (Data[4] % 32) + 1 : 1);
  png_uint_32 height = (Size > 5 ? (Data[5] % 32) + 1 : 1);

  int color_type_options[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGBA
  };
  int bit_depth_options[] = {1, 2, 4, 8, 16};

  int color_type = color_type_options[(Size > 6 ? Data[6] : 0) % 5];
  int bit_depth = bit_depth_options[(Size > 7 ? Data[7] : 0) % 5];

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
      color_type == PNG_COLOR_TYPE_RGBA) {
    if (bit_depth < 8) bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  int interlace = (Size > 8 && (Data[8] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_method = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression_type, filter_method);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = (Size > 9 ? (Data[9] % 256) : 1);
    if (palette_entries <= 0) palette_entries = 1;
    for (int i = 0; i < palette_entries; ++i) {
      uint8_t v = (Size > 10 + (size_t)i ? Data[10 + i] : (uint8_t)i);
      palette[i].red = v;
      palette[i].green = static_cast<png_byte>(v ^ 0x55);
      palette[i].blue = static_cast<png_byte>(v ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGBA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2 : 1;
  size_t rowbytes;
  if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) {
    rowbytes = ((size_t)width * (size_t)bit_depth * (size_t)channels + 7u) / 8u;
  } else {
    rowbytes = (size_t)width * (size_t)channels * bytes_per_sample;
  }

  png_bytep image_data = nullptr;
  png_bytep* rows = nullptr;

  if (height > 0 && rowbytes > 0) {
    image_data = reinterpret_cast<png_bytep>(malloc(rowbytes * height));
    rows = reinterpret_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
  }

  if (image_data == nullptr || rows == nullptr) {
    free(image_data);
    free(rows);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image_data + y * rowbytes;
    for (size_t x = 0; x < rowbytes; ++x) {
      size_t idx = (size_t)y * rowbytes + x;
      rows[y][x] = (Size > 0) ? Data[idx % Size] : 0;
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int level = 0;
  if (Size > 11) {
    level = (int)((int8_t)Data[11]);
  }
  png_set_compression_level(png_ptr, level);
  png_set_compression_level(nullptr, level);

  int strategy_values[] = {
#ifdef Z_DEFAULT_STRATEGY
      Z_DEFAULT_STRATEGY,
#else
      0,
#endif
#ifdef Z_FILTERED
      Z_FILTERED,
#else
      1,
#endif
#ifdef Z_HUFFMAN_ONLY
      Z_HUFFMAN_ONLY,
#else
      2,
#endif
#ifdef Z_RLE
      Z_RLE,
#else
      3,
#endif
#ifdef Z_FIXED
      Z_FIXED
#else
      4
#endif
  };
  int strategy = strategy_values[(Size > 12 ? Data[12] : 0) % 5];
  png_set_compression_strategy(png_ptr, strategy);
  png_set_compression_strategy(nullptr, strategy);

  int heuristic_method = (Size > 13 ? Data[13] % PNG_FILTER_HEURISTIC_LAST : PNG_FILTER_HEURISTIC_DEFAULT);
  int num_weights = (Size > 14 ? Data[14] % 8 : 0);

  double dweights[8];
  double dcosts[8];
  png_fixed_point fweights[8];
  png_fixed_point fcosts[8];
  for (int i = 0; i < 8; ++i) {
    uint8_t a = (Size > (size_t)(15 + i) ? Data[15 + i] : (uint8_t)i);
    uint8_t b = (Size > (size_t)(23 + i) ? Data[23 + i] : (uint8_t)(i * 3));
    dweights[i] = (double)(a % 100) / 10.0;
    dcosts[i] = (double)(b % 100) / 10.0;
    fweights[i] = (png_fixed_point)(a * 1000);
    fcosts[i] = (png_fixed_point)(b * 1000);
  }

  png_set_filter_heuristics(png_ptr, heuristic_method, num_weights, dweights, dcosts);
  png_set_filter_heuristics(png_ptr, heuristic_method, 0, nullptr, nullptr);
  png_set_filter_heuristics(nullptr, heuristic_method, num_weights, dweights, dcosts);

  png_set_filter_heuristics_fixed(png_ptr, heuristic_method, num_weights, fweights, fcosts);
  png_set_filter_heuristics_fixed(png_ptr, heuristic_method, 0, nullptr, nullptr);
  png_set_filter_heuristics_fixed(nullptr, heuristic_method, num_weights, fweights, fcosts);

  int flush_rows = 0;
  if (Size > 31) {
    flush_rows = (int)((int8_t)Data[31]);
  }
  png_set_flush(png_ptr, flush_rows);
  png_set_flush(nullptr, flush_rows);

  int transforms = 0;
  if (Size > 32) {
    uint8_t t = Data[32];
#ifdef PNG_TRANSFORM_PACKING
    if (t & 0x01) transforms |= PNG_TRANSFORM_PACKING;
#endif
#ifdef PNG_TRANSFORM_PACKSWAP
    if (t & 0x02) transforms |= PNG_TRANSFORM_PACKSWAP;
#endif
#ifdef PNG_TRANSFORM_SHIFT
    if (t & 0x04) transforms |= PNG_TRANSFORM_SHIFT;
#endif
#ifdef PNG_TRANSFORM_SWAP_ALPHA
    if (t & 0x08) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_BGR
    if (t & 0x10) transforms |= PNG_TRANSFORM_BGR;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
    if (t & 0x20) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
    if (t & 0x40) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_INVERT_MONO
    if (t & 0x80) transforms |= PNG_TRANSFORM_INVERT_MONO;
#endif
  }

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  free(rows);
  free(image_data);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}