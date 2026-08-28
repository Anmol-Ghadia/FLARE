// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
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

static void PngWriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == nullptr || fwrite(data, 1, length, fp) != length) {
    png_error(png_ptr, "write error");
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local8[8] = {0};
  uint8_t local4[4] = {0};
  uint8_t local2[2] = {0};

  if (Size > 0) {
    memcpy(local8, Data, Size > sizeof(local8) ? sizeof(local8) : Size);
  }

  png_uint_16 v16 = png_get_uint_16(local8);
  png_uint_32 v32 = png_get_uint_32(local8);

  png_save_uint_16(local2, static_cast<unsigned int>(v16));
  png_save_uint_32(local4, v32);

  if (Size >= 2) {
    png_uint_16 v16b = png_get_uint_16(Data);
    png_save_uint_16(local2, static_cast<unsigned int>(v16b));
  }

  if (Size >= 4) {
    png_uint_32 v32b = png_get_uint_32(Data);
    png_save_uint_32(local4, v32b);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteDataFn, PngFlushFn);

  png_byte chunk_name[4] = {'t', 'E', 'S', 'T'};
  if (Size >= 4) {
    memcpy(chunk_name, Data, 4);
  }

  png_byte chunk_data_small[8] = {0};
  size_t small_len = Size > sizeof(chunk_data_small) ? sizeof(chunk_data_small) : Size;
  if (small_len > 0) {
    memcpy(chunk_data_small, Data, small_len);
  }

  png_write_chunk(png_ptr, chunk_name, nullptr, 0);
  png_write_chunk(png_ptr, chunk_name, chunk_data_small, small_len);

  png_byte derived_chunk1[6];
  memcpy(derived_chunk1, local2, 2);
  memcpy(derived_chunk1 + 2, local4, 4);
  png_write_chunk(png_ptr, chunk_name, derived_chunk1, sizeof(derived_chunk1));

  png_uint_32 len31 = 0;
  if (Size >= 4) {
    len31 = png_get_uint_31(png_ptr, Data);
  } else {
    len31 = png_get_uint_31(png_ptr, local4);
  }

  size_t bounded_len = static_cast<size_t>(len31 % 32U);
  png_byte chunk_data_var[32] = {0};
  size_t copy_len = Size > bounded_len ? bounded_len : Size;
  if (copy_len > 0) {
    memcpy(chunk_data_var, Data, copy_len);
  }
  png_write_chunk(png_ptr, chunk_name, chunk_data_var, bounded_len);

  png_destroy_write_struct(&png_ptr, nullptr);
  fclose(fp);
  return 0;
}