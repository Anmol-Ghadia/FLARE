// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
// png_set_text_compression_strategy at pngwrite.c:1261:1 in png.h
// png_set_text_compression_method at pngwrite.c:1296:1 in png.h
// png_set_text_compression_mem_level at pngwrite.c:1250:1 in png.h
// png_set_compression_level at pngwrite.c:1153:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_text_compression_level at pngwrite.c:1239:1 in png.h
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
#include <stdio.h>
#include <string.h>

static png_uint_32 ReadU32(const uint8_t *data) {
  return (static_cast<png_uint_32>(data[0]) << 24) |
         (static_cast<png_uint_32>(data[1]) << 16) |
         (static_cast<png_uint_32>(data[2]) << 8) |
         (static_cast<png_uint_32>(data[3]));
}

static int ReadS32(const uint8_t *data) {
  return static_cast<int>(ReadU32(data));
}

extern "C" int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png_ptr == NULL) {
    return 0;
  }

  if (Size < 24) {
    png_set_compression_level(png_ptr, 0);
    png_set_text_compression_window_bits(png_ptr, 8);
    png_set_text_compression_level(png_ptr, 0);
    png_set_text_compression_strategy(png_ptr, 0);
    png_set_text_compression_method(png_ptr, 8);
    png_set_text_compression_mem_level(png_ptr, 8);

    png_set_compression_level(NULL, 1);
    png_set_text_compression_window_bits(NULL, 9);
    png_set_text_compression_level(NULL, 2);
    png_set_text_compression_strategy(NULL, 3);
    png_set_text_compression_method(NULL, 4);
    png_set_text_compression_mem_level(NULL, 5);

    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  size_t off = 0;
  int compression_level = ReadS32(Data + off);
  off += 4;
  int text_window_bits = ReadS32(Data + off);
  off += 4;
  int text_level = ReadS32(Data + off);
  off += 4;
  int text_strategy = ReadS32(Data + off);
  off += 4;
  int text_method = ReadS32(Data + off);
  off += 4;
  int text_mem_level = ReadS32(Data + off);
  off += 4;

  png_set_compression_level(png_ptr, compression_level);
  png_set_text_compression_window_bits(png_ptr, text_window_bits);
  png_set_text_compression_level(png_ptr, text_level);
  png_set_text_compression_strategy(png_ptr, text_strategy);
  png_set_text_compression_method(png_ptr, text_method);
  png_set_text_compression_mem_level(png_ptr, text_mem_level);

  if (Size > off) {
    const uint8_t selector = Data[off++];

    switch (selector % 4) {
      case 0:
        png_set_compression_level(png_ptr, -1);
        png_set_text_compression_window_bits(png_ptr, 7);
        png_set_text_compression_level(png_ptr, -1);
        png_set_text_compression_strategy(png_ptr, -1);
        png_set_text_compression_method(png_ptr, 7);
        png_set_text_compression_mem_level(png_ptr, -1);
        break;
      case 1:
        png_set_compression_level(png_ptr, 9);
        png_set_text_compression_window_bits(png_ptr, 15);
        png_set_text_compression_level(png_ptr, 9);
        png_set_text_compression_strategy(png_ptr, 4);
        png_set_text_compression_method(png_ptr, 8);
        png_set_text_compression_mem_level(png_ptr, 9);
        break;
      case 2:
        png_set_compression_level(png_ptr, 1000000);
        png_set_text_compression_window_bits(png_ptr, 1000000);
        png_set_text_compression_level(png_ptr, 1000000);
        png_set_text_compression_strategy(png_ptr, 1000000);
        png_set_text_compression_method(png_ptr, 1000000);
        png_set_text_compression_mem_level(png_ptr, 1000000);
        break;
      case 3:
        png_set_compression_level(png_ptr, -1000000);
        png_set_text_compression_window_bits(png_ptr, -1000000);
        png_set_text_compression_level(png_ptr, -1000000);
        png_set_text_compression_strategy(png_ptr, -1000000);
        png_set_text_compression_method(png_ptr, -1000000);
        png_set_text_compression_mem_level(png_ptr, -1000000);
        break;
    }
  }

  while (off + 4 <= Size) {
    int v = ReadS32(Data + off);
    off += 4;
    uint8_t which = static_cast<uint8_t>(v);

    switch (which % 6) {
      case 0:
        png_set_compression_level(png_ptr, v);
        break;
      case 1:
        png_set_text_compression_window_bits(png_ptr, v);
        break;
      case 2:
        png_set_text_compression_level(png_ptr, v);
        break;
      case 3:
        png_set_text_compression_strategy(png_ptr, v);
        break;
      case 4:
        png_set_text_compression_method(png_ptr, v);
        break;
      case 5:
        png_set_text_compression_mem_level(png_ptr, v);
        break;
    }
  }

  png_set_compression_level(NULL, compression_level);
  png_set_text_compression_window_bits(NULL, text_window_bits);
  png_set_text_compression_level(NULL, text_level);
  png_set_text_compression_strategy(NULL, text_strategy);
  png_set_text_compression_method(NULL, text_method);
  png_set_text_compression_mem_level(NULL, text_mem_level);

  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}