// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_compression_window_bits at pngwrite.c:1192:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_82(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == NULL) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    remove("./dummy_file");
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint32_t seed = 0;
  for (size_t i = 0; i < Size && i < 4; ++i) {
    seed = (seed << 8) | Data[i];
  }

  int compression_level = (Size > 4) ? (int)(int8_t)Data[4] : (int)(seed & 0xff);
  int window_bits = (Size > 5) ? (int)(int8_t)Data[5] : (int)((seed >> 8) & 0xff);
  int flush_rows = (Size > 6) ? (int)(int8_t)Data[6] : (int)((seed >> 16) & 0xff);
  int method = (Size > 7) ? (int)(Data[7] % 4) : 0;
  int filters = (Size > 8) ? (int)Data[8] : (int)((seed >> 24) & 0xff);

  png_set_compression_level(png_ptr, compression_level);
  png_set_compression_window_bits(png_ptr, window_bits);
  png_set_flush(png_ptr, flush_rows);
  png_set_filter(png_ptr, method, filters);

  png_uint_32 width = 1;
  png_uint_32 height = 1;
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace_type = PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;

  if (Size > 9) {
    width = (png_uint_32)(Data[9] % 32) + 1;
  }
  if (Size > 10) {
    height = (png_uint_32)(Data[10] % 32) + 1;
  }
  if (Size > 11) {
    switch (Data[11] % 5) {
      case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
      case 1: color_type = PNG_COLOR_TYPE_RGB; break;
      case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
      case 3: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      default: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    }
  }
  if (Size > 12) {
    switch (Data[12] % 4) {
      case 0: bit_depth = 1; break;
      case 1: bit_depth = 2; break;
      case 2: bit_depth = 4; break;
      default: bit_depth = 8; break;
    }
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    bit_depth = (Size > 13 && (Data[13] & 1)) ? 16 : 8;
  } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
    if (bit_depth != 1 && bit_depth != 2 && bit_depth != 4 && bit_depth != 8) {
      bit_depth = 8;
    }
  } else if (color_type == PNG_COLOR_TYPE_GRAY) {
    if (bit_depth != 1 && bit_depth != 2 && bit_depth != 4 &&
        bit_depth != 8 && bit_depth != 16) {
      bit_depth = 8;
    }
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_type);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    for (int i = 0; i < 256; ++i) {
      palette[i].red = (png_byte)i;
      palette[i].green = (png_byte)(255 - i);
      palette[i].blue = (png_byte)(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
  }

  png_write_info(png_ptr, info_ptr);

  int channels = 3;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 3; break;
  }

  size_t rowbytes = (size_t)(((uint64_t)width * (uint64_t)channels * (uint64_t)bit_depth + 7u) / 8u);
  if (rowbytes == 0) {
    rowbytes = 1;
  }

  png_bytep row = (png_bytep)malloc(rowbytes);
  if (row == NULL) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    remove("./dummy_file");
    return 0;
  }

  size_t offset = 14;
  for (png_uint_32 y = 0; y < height; ++y) {
    for (size_t x = 0; x < rowbytes; ++x) {
      if (offset < Size) {
        row[x] = Data[offset++];
      } else {
        row[x] = (png_byte)((x + y) & 0xff);
      }
    }

    png_set_compression_level(png_ptr, compression_level + (int)y);
    png_set_compression_window_bits(png_ptr, window_bits + (int)(y % 5) - 2);
    png_set_flush(png_ptr, flush_rows - (int)y);
    png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters ^ (int)y);

    png_write_row(png_ptr, row);
  }

  png_write_end(png_ptr, info_ptr);

  free(row);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  remove("./dummy_file");
  return 0;
}