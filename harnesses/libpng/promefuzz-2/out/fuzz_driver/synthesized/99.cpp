// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_pCAL at pngset.c:312:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void PngWriteFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == NULL || fwrite(data, 1, length, fp) != length) {
    png_error(png_ptr, "write failed");
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != NULL) fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput_99(const uint8_t *Data, size_t Size) {
  png_byte intbuf[4] = {0, 0, 0, 0};
  if (Size >= 4) {
    memcpy(intbuf, Data, 4);
  } else if (Size > 0) {
    memcpy(intbuf, Data, Size);
  }

  png_int_32 parsed = png_get_int_32(intbuf);
  png_byte saved[4] = {0, 0, 0, 0};
  png_save_int_32(saved, parsed);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
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

  png_set_write_fn(png_ptr, fp, PngWriteFn, PngFlushFn);

  int width = 1 + (Size > 4 ? (Data[4] % 8) : 0);
  int height = 1 + (Size > 5 ? (Data[5] % 8) : 0);
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace = (Size > 6 && (Data[6] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_int_32 off_x = parsed;
  png_int_32 off_y = png_get_int_32(saved);
  int unit_type = (Size > 7) ? (Data[7] % 3) : 0;
  png_set_oFFs(png_ptr, info_ptr, off_x, off_y, unit_type);

  char purpose_buf[32];
  char units_buf[32];
  snprintf(purpose_buf, sizeof(purpose_buf), "p%08x", (unsigned)(uint32_t)parsed);
  snprintf(units_buf, sizeof(units_buf), "u%u", (unsigned)(Size & 0xffff));

  char param_storage[4][32];
  char* params[4];
  int nparams = (Size > 8) ? (Data[8] % 5) : 0;
  for (int i = 0; i < 4; ++i) {
    snprintf(param_storage[i], sizeof(param_storage[i]), "%u",
             (unsigned)((i < (int)Size) ? Data[i] : i));
    params[i] = param_storage[i];
  }
  int pcal_type = (Size > 9) ? (Data[9] % 6) : 0;
  png_set_pCAL(png_ptr, info_ptr, purpose_buf, off_x, off_y, pcal_type,
               nparams, units_buf, params);

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'z'};
  if (Size >= 12) {
    memcpy(chunk_name, Data + 8, 4);
  }

  png_const_bytep chunk_data = Size ? reinterpret_cast<png_const_bytep>(Data) : NULL;
  size_t chunk_len = Size;
  if (chunk_len > 1024) chunk_len = 1024;
  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  png_byte row[8 * 3];
  memset(row, 0, sizeof(row));
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width * 3; ++x) {
      row[x] = (Size > 0) ? Data[(x + y) % Size] : 0;
    }
    png_write_row(png_ptr, row);
  }

  png_write_end(png_ptr, info_ptr);

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  remove("./dummy_file");
  return 0;
}