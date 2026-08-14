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
  (void)png_ptr;
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL && data != NULL && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    remove("./dummy_file");
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == NULL) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint32_t seed = 0;
  for (size_t i = 0; i < Size && i < 4; ++i) {
    seed = (seed << 8) | Data[i];
  }

  png_uint_32 width = (Size > 0 ? (png_uint_32)(Data[0] % 8 + 1) : 1);
  png_uint_32 height = (Size > 1 ? (png_uint_32)(Data[1] % 8 + 1) : 1);

  int color_type;
  switch (Size > 2 ? (Data[2] % 5) : 0) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
    case 2: color_type = PNG_COLOR_TYPE_RGB; break;
    case 3: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    default: color_type = PNG_COLOR_TYPE_PALETTE; break;
  }

  int bit_depth_candidates[] = {1, 2, 4, 8, 16};
  int bit_depth = bit_depth_candidates[Size > 3 ? (Data[3] % 5) : 0];

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_size = (bit_depth == 1) ? 2 : (bit_depth == 2) ? 4 : (bit_depth == 4) ? 16 : 256;
    for (int i = 0; i < palette_size; ++i) {
      uint8_t v = (Size > 4 + (size_t)i) ? Data[4 + i] : (uint8_t)i;
      palette[i].red = v;
      palette[i].green = (uint8_t)(v ^ 0x55);
      palette[i].blue = (uint8_t)(v ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
  }

  png_set_benign_errors(png_ptr, Size > 4 ? (int)(Data[4] & 1) : 0);
  png_set_bgr(png_ptr);
  png_set_packing(png_ptr);

  int flush_rows = 0;
  if (Size > 5) {
    flush_rows = (int)((int8_t)Data[5]);
  }
  png_set_flush(png_ptr, flush_rows);

  png_fixed_point screen_gamma = (Size > 9) ? (png_fixed_point)((uint32_t)Data[6] << 24 |
                                                                 (uint32_t)Data[7] << 16 |
                                                                 (uint32_t)Data[8] << 8 |
                                                                 (uint32_t)Data[9]) : 100000;
  png_fixed_point file_gamma = (Size > 13) ? (png_fixed_point)((uint32_t)Data[10] << 24 |
                                                                (uint32_t)Data[11] << 16 |
                                                                (uint32_t)Data[12] << 8 |
                                                                (uint32_t)Data[13]) : 100000;
  png_set_gamma_fixed(png_ptr, screen_gamma, file_gamma);

  int channels;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    default: channels = 1; break;
  }

  size_t rowbytes;
  if (bit_depth < 8) {
    rowbytes = ((size_t)width * (size_t)bit_depth * (size_t)channels + 7u) / 8u;
  } else {
    rowbytes = (size_t)width * (size_t)channels * (size_t)(bit_depth / 8);
  }

  if (rowbytes == 0) rowbytes = 1;

  png_bytep image = (png_bytep)malloc(rowbytes * height);
  png_bytepp rows = (png_bytepp)malloc(sizeof(png_bytep) * height);
  if (image == NULL || rows == NULL) {
    free(image);
    free(rows);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    remove("./dummy_file");
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image + y * rowbytes;
    for (size_t x = 0; x < rowbytes; ++x) {
      size_t idx = 14 + ((size_t)y * rowbytes + x);
      rows[y][x] = (Size > idx) ? Data[idx] : (uint8_t)((seed + y + x) & 0xFF);
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int transforms = 0;
  if (Size > 14) {
    if (Data[14] & 0x01) transforms |= PNG_TRANSFORM_BGR;
    if (Data[14] & 0x02) transforms |= PNG_TRANSFORM_PACKING;
    if (Data[14] & 0x04) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
    if (Data[14] & 0x08) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
    if (Data[14] & 0x10) transforms |= PNG_TRANSFORM_STRIP_FILLER_AFTER;
    if (Data[14] & 0x20) transforms |= PNG_TRANSFORM_STRIP_FILLER_BEFORE;
    if (Data[14] & 0x40) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
    if (Data[14] & 0x80) transforms |= PNG_TRANSFORM_INVERT_MONO;
  }

  png_write_png(png_ptr, info_ptr, transforms, NULL);

  free(image);
  free(rows);
  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  remove("./dummy_file");
  return 0;
}