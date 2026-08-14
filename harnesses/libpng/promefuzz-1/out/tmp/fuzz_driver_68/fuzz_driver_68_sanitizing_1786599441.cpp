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

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  uint32_t v0 = 0, v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0;
  if (Size > 0) v0 = Data[0];
  if (Size > 1) v1 = Data[1];
  if (Size > 2) v2 = Data[2];
  if (Size > 3) v3 = Data[3];
  if (Size > 4) v4 = Data[4];
  if (Size > 5) v5 = Data[5];

  png_uint_32 width = (Size > 6) ? ((v0 << 8) | v1) : 1;
  png_uint_32 height = (Size > 6) ? ((v2 << 8) | v3) : 1;
  width = (width % 32) + 1;
  height = (height % 32) + 1;

  int color_type;
  switch (v4 % 5) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
    case 2: color_type = PNG_COLOR_TYPE_RGB; break;
    case 3: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    default: color_type = PNG_COLOR_TYPE_PALETTE; break;
  }

  int bit_depth;
  switch (v5 % 4) {
    case 0: bit_depth = 1; break;
    case 1: bit_depth = 2; break;
    case 2: bit_depth = 4; break;
    default: bit_depth = 8; break;
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = 1 << bit_depth;
    for (int i = 0; i < palette_entries; ++i) {
      uint8_t b = (Size > 0) ? Data[(6 + i) % Size] : static_cast<uint8_t>(i);
      palette[i].red = static_cast<png_byte>(b);
      palette[i].green = static_cast<png_byte>(b ^ 0x55);
      palette[i].blue = static_cast<png_byte>(b ^ 0xaa);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bits_per_pixel = static_cast<size_t>(channels * bit_depth);
  png_size_t rowbytes = static_cast<png_size_t>((width * bits_per_pixel + 7) / 8);

  png_bytep image_data = nullptr;
  png_bytep* rows = nullptr;

  if (rowbytes == 0) {
    rowbytes = 1;
  }

  image_data = static_cast<png_bytep>(malloc(static_cast<size_t>(rowbytes) * height));
  rows = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));

  if (image_data == nullptr || rows == nullptr) {
    free(image_data);
    free(rows);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image_data + y * rowbytes;
    for (png_size_t x = 0; x < rowbytes; ++x) {
      if (Size > 0) {
        rows[y][x] = Data[(6 + y * rowbytes + x) % Size];
      } else {
        rows[y][x] = 0;
      }
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int compression_level = 0;
  int compression_strategy = 0;
  int flush_rows = 0;
  int heuristic_method = PNG_FILTER_HEURISTIC_DEFAULT;
  int num_weights = 0;
  int transforms = 0;

  if (Size > 10) compression_level = static_cast<int8_t>(Data[6]);
  if (Size > 11) compression_strategy = static_cast<int8_t>(Data[7]);
  if (Size > 12) flush_rows = static_cast<int8_t>(Data[8]);
  if (Size > 13) heuristic_method = Data[9] % PNG_FILTER_HEURISTIC_LAST;
  if (Size > 14) num_weights = Data[10] % 8;
  if (Size > 15) transforms = Data[11];

  png_set_compression_level(png_ptr, compression_level);
  png_set_compression_strategy(png_ptr, compression_strategy);
  png_set_flush(png_ptr, flush_rows);

  double dweights[8];
  double dcosts[8];
  png_fixed_point fweights[8];
  png_fixed_point fcosts[8];
  for (int i = 0; i < 8; ++i) {
    uint8_t b1 = (Size > 0) ? Data[(12 + i) % Size] : 0;
    uint8_t b2 = (Size > 0) ? Data[(20 + i) % Size] : 0;
    dweights[i] = static_cast<double>(b1) / 255.0;
    dcosts[i] = static_cast<double>(b2) / 255.0;
    fweights[i] = static_cast<png_fixed_point>(b1 * 1000);
    fcosts[i] = static_cast<png_fixed_point>(b2 * 1000);
  }

  png_set_filter_heuristics(png_ptr, heuristic_method, num_weights,
                            dweights, dcosts);
  png_set_filter_heuristics_fixed(png_ptr, heuristic_method, num_weights,
                                  fweights, fcosts);

  png_set_compression_level(nullptr, compression_level);
  png_set_compression_strategy(nullptr, compression_strategy);
  png_set_flush(nullptr, flush_rows);
  png_set_filter_heuristics(nullptr, heuristic_method, num_weights,
                            dweights, dcosts);
  png_set_filter_heuristics_fixed(nullptr, heuristic_method, num_weights,
                                  fweights, fcosts);

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  free(rows);
  free(image_data);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}