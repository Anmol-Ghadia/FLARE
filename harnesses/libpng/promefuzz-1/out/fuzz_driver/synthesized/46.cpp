// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_strategy at pngwrite.c:1175:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
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
#include <setjmp.h>
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_46(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                PngErrorFn, PngWarningFn);
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
    return 0;
  }

  size_t off = 0;

  auto next_u8 = [&](uint8_t defval) -> uint8_t {
    if (off < Size) return Data[off++];
    return defval;
  };

  auto next_u32 = [&](uint32_t defval) -> uint32_t {
    if (off + 4 <= Size) {
      uint32_t v = (uint32_t)Data[off] |
                   ((uint32_t)Data[off + 1] << 8) |
                   ((uint32_t)Data[off + 2] << 16) |
                   ((uint32_t)Data[off + 3] << 24);
      off += 4;
      return v;
    }
    return defval;
  };

  auto next_size = [&](size_t defval) -> size_t {
    if (off + sizeof(size_t) <= Size) {
      size_t v = 0;
      memcpy(&v, Data + off, sizeof(size_t));
      off += sizeof(size_t);
      return v;
    }
    return defval;
  };

  auto next_int = [&](int defval) -> int {
    if (off + sizeof(int) <= Size) {
      int v = 0;
      memcpy(&v, Data + off, sizeof(int));
      off += sizeof(int);
      return v;
    }
    return defval;
  };

  png_set_compression_buffer_size(NULL, 0);
  png_set_compression_strategy(NULL, 0);
  png_set_compression_method(NULL, 0);
  png_set_compression_mem_level(NULL, 0);
  png_set_flush(NULL, 0);
  png_set_filter(NULL, 0, 0);

  size_t buf_size1 = next_size(1);
  size_t buf_size2 = next_size(8192);
  int strategy1 = next_int(0);
  int strategy2 = next_int(1);
  int method1 = next_int(8);
  int method2 = next_int(-1);
  int mem_level1 = next_int(8);
  int mem_level2 = next_int(1);
  int flush1 = next_int(0);
  int flush2 = next_int(1);
  int filter_method1 = next_int(0);
  int filter_method2 = next_int(1);
  int filters1 = next_int(PNG_ALL_FILTERS);
  int filters2 = next_int(0);

  png_set_compression_buffer_size(png_ptr, buf_size1);
  png_set_compression_strategy(png_ptr, strategy1);
  png_set_compression_method(png_ptr, method1);
  png_set_compression_mem_level(png_ptr, mem_level1);
  png_set_flush(png_ptr, flush1);
  png_set_filter(png_ptr, filter_method1, filters1);

  png_set_compression_buffer_size(png_ptr, buf_size2);
  png_set_compression_strategy(png_ptr, strategy2);
  png_set_compression_method(png_ptr, method2);
  png_set_compression_mem_level(png_ptr, mem_level2);
  png_set_flush(png_ptr, flush2);
  png_set_filter(png_ptr, filter_method2, filters2);

  int choice = next_u8(0) % 6;
  switch (choice) {
    case 0:
      png_set_compression_buffer_size(png_ptr, 0);
      break;
    case 1:
      png_set_compression_buffer_size(png_ptr, (size_t)PNG_UINT_31_MAX + 1u);
      break;
    case 2:
      png_set_compression_method(png_ptr, 8);
      png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_NO_FILTERS);
      break;
    case 3:
      png_set_compression_strategy(png_ptr, -1);
      png_set_flush(png_ptr, -1);
      break;
    case 4:
      png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);
      png_set_compression_mem_level(png_ptr, 9);
      break;
    case 5:
      png_set_filter(png_ptr, 99, 0x7fffffff);
      break;
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}