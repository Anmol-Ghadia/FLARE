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

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t& off) {
  if (off + 4 > size) return 0;
  png_uint_32 v = (static_cast<png_uint_32>(data[off]) << 24) |
                  (static_cast<png_uint_32>(data[off + 1]) << 16) |
                  (static_cast<png_uint_32>(data[off + 2]) << 8) |
                  (static_cast<png_uint_32>(data[off + 3]));
  off += 4;
  return v;
}

static int ReadS32(const uint8_t* data, size_t size, size_t& off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static void WriteDataToFile(const uint8_t* data, size_t size) {
  FILE* f = fopen("./dummy_file", "wb");
  if (!f) return;
  if (size > 0) fwrite(data, 1, size, f);
  fclose(f);
}

static void NoOpFlush(png_structp) {
}

static png_bytep* BuildRows(const uint8_t* data, size_t size, size_t payload_off,
                            png_uint_32 width, png_uint_32 height,
                            int color_type, int bit_depth) {
  int channels = 0;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    default: return NULL;
  }

  if (width == 0 || height == 0) return NULL;

  png_size_t rowbytes = 0;
  if (bit_depth >= 8) {
    png_uint_32 bytes_per_sample = static_cast<png_uint_32>(bit_depth / 8);
    if (bytes_per_sample == 0) return NULL;
    if (width > 4096 || height > 4096) return NULL;
    rowbytes = static_cast<png_size_t>(width) * channels * bytes_per_sample;
  } else {
    if (width > 4096 || height > 4096) return NULL;
    png_uint_32 bits = width * channels * static_cast<png_uint_32>(bit_depth);
    rowbytes = (bits + 7) / 8;
  }

  if (rowbytes == 0 || rowbytes > (1u << 20) || height > 1024) return NULL;

  png_bytep* rows = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
  if (!rows) return NULL;

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = static_cast<png_bytep>(malloc(rowbytes));
    if (!rows[y]) {
      for (png_uint_32 i = 0; i < y; ++i) free(rows[i]);
      free(rows);
      return NULL;
    }
    for (png_size_t x = 0; x < rowbytes; ++x) {
      size_t idx = payload_off + ((static_cast<size_t>(y) * rowbytes + x) % (size ? size : 1));
      rows[y][x] = size ? data[idx % size] : 0;
    }
  }

  return rows;
}

static void FreeRows(png_bytep* rows, png_uint_32 height) {
  if (!rows) return;
  for (png_uint_32 y = 0; y < height; ++y) free(rows[y]);
  free(rows);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDataToFile(Data, Size);

  size_t off = 0;
  png_uint_32 width = (ReadU32(Data, Size, off) % 64) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 64) + 1;

  static const int bit_depths[] = {1, 2, 4, 8, 16};
  static const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  static const int interlaces[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};

  int bit_depth = bit_depths[(Size > off ? Data[off++] : 0) % (sizeof(bit_depths) / sizeof(bit_depths[0]))];
  int color_type = color_types[(Size > off ? Data[off++] : 0) % (sizeof(color_types) / sizeof(color_types[0]))];
  int interlace = interlaces[(Size > off ? Data[off++] : 0) % (sizeof(interlaces) / sizeof(interlaces[0]))];
  int compression = (Size > off ? Data[off++] : 0) & 1 ? PNG_COMPRESSION_TYPE_BASE : 99;
  int filter = (Size > off ? Data[off++] : 0) & 1 ? PNG_FILTER_TYPE_BASE : 99;
  int srgb_intent = ReadS32(Data, Size, off);
  int transforms = ReadS32(Data, Size, off);

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;

  FILE* fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_flush_fn(png_ptr, NULL, NoOpFlush);

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  if (Size > off) {
    if (Data[off++] & 1)
      png_set_sRGB(png_ptr, info_ptr, srgb_intent);
    else
      png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
  } else {
    png_set_sRGB(png_ptr, info_ptr, srgb_intent);
  }

  png_bytep* rows = BuildRows(Data, Size, off, width, height, color_type, bit_depth);

  if (rows) {
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_color palette[256];
      for (int i = 0; i < 256; ++i) {
        uint8_t v = Size ? Data[(off + i) % Size] : 0;
        palette[i].red = v;
        palette[i].green = static_cast<png_byte>(v ^ 0x55);
        palette[i].blue = static_cast<png_byte>(v ^ 0xAA);
      }
      png_set_PLTE(png_ptr, info_ptr, palette, 256);
    }

    png_set_rows(png_ptr, info_ptr, rows);

    if (Size > off && (Data[off++] & 1)) {
      png_write_info_before_PLTE(png_ptr, info_ptr);
      png_write_png(png_ptr, info_ptr, transforms, NULL);
    } else {
      png_write_png(png_ptr, info_ptr, transforms, NULL);
    }
  } else {
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  png_write_end(png_ptr, info_ptr);

  FreeRows(rows, height);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}