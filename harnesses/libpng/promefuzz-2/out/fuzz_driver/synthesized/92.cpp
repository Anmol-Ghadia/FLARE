// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
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
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb) longjmp(*jb, 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_92(const uint8_t *Data, size_t Size) {
  uint8_t local8[8] = {0};
  uint8_t local4[4] = {0};
  uint8_t local2[2] = {0};

  if (Size > 0) {
    memcpy(local8, Data, Size > sizeof(local8) ? sizeof(local8) : Size);
  }

  png_uint_16 v16 = 0;
  png_uint_32 v32 = 0;
  png_uint_32 v31 = 0;

  if (Size >= 2) {
    v16 = png_get_uint_16(local8);
    png_save_uint_16(local2, v16);
    (void)png_get_uint_16(local2);
  } else {
    png_save_uint_16(local2, 0);
    (void)png_get_uint_16(local2);
  }

  if (Size >= 4) {
    v32 = png_get_uint_32(local8);
    png_save_uint_32(local4, v32);
    (void)png_get_uint_32(local4);
  } else {
    png_save_uint_32(local4, 0);
    (void)png_get_uint_32(local4);
  }

  jmp_buf read_jmp;
  png_structp read_png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                &read_jmp, PngErrorFn, PngWarningFn);
  if (read_png != nullptr) {
    if (setjmp(read_jmp) == 0) {
      uint8_t buf31[4] = {0, 0, 0, 0};
      if (Size >= 4) {
        memcpy(buf31, Data, 4);
      } else if (Size > 0) {
        memcpy(buf31, Data, Size);
      }
      v31 = png_get_uint_31(read_png, buf31);
      png_save_uint_32(local4, v31);
      (void)png_get_uint_32(local4);
    }
    png_destroy_read_struct(&read_png, nullptr, nullptr);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  jmp_buf write_jmp;
  png_structp write_png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  &write_jmp, PngErrorFn, PngWarningFn);
  if (write_png == nullptr) {
    fclose(fp);
    return 0;
  }

  if (setjmp(write_jmp) != 0) {
    png_destroy_write_struct(&write_png, nullptr);
    fclose(fp);
    return 0;
  }

  png_init_io(write_png, fp);

  uint8_t chunk_name[4] = {'t', 'E', 'S', 'T'};
  if (Size >= 4) {
    memcpy(chunk_name, Data, 4);
  }

  const png_bytep chunk_data =
      Size > 4 ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + 4))
               : nullptr;
  size_t chunk_len = Size > 4 ? Size - 4 : 0;

  png_write_chunk(write_png, chunk_name, chunk_data, chunk_len);

  png_write_chunk(write_png,
                  reinterpret_cast<png_const_bytep>("IEND"),
                  nullptr, 0);

  png_save_uint_16(local2, static_cast<unsigned int>(Size & 0xFFFFu));
  png_save_uint_32(local4, static_cast<png_uint_32>(Size));
  (void)png_get_uint_16(local2);
  (void)png_get_uint_32(local4);

  png_destroy_write_struct(&write_png, nullptr);
  fclose(fp);
  return 0;
}