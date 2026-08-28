// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
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
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};

  png_uint_32 u32 = 0;
  png_int_32 i32 = 0;
  unsigned int u16 = 0;

  if (Size >= 4) {
    u32 = (static_cast<png_uint_32>(Data[0]) << 24) |
          (static_cast<png_uint_32>(Data[1]) << 16) |
          (static_cast<png_uint_32>(Data[2]) << 8) |
          static_cast<png_uint_32>(Data[3]);
    i32 = static_cast<png_int_32>(u32);
  } else {
    for (size_t i = 0; i < Size; ++i) {
      u32 = (u32 << 8) | Data[i];
    }
    i32 = static_cast<png_int_32>(u32);
  }

  if (Size >= 2) {
    u16 = (static_cast<unsigned int>(Data[0]) << 8) |
          static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    u16 = Data[0];
  }

  png_save_uint_32(buf4, u32);
  (void)png_get_int_32(buf4);

  png_save_int_32(buf4, i32);
  (void)png_get_int_32(buf4);

  png_save_uint_16(buf2, u16);

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else {
    memset(buf4, 0, sizeof(buf4));
    memcpy(buf4, Data, Size);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr != NULL) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      (void)png_get_uint_31(png_ptr, buf4);
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
      if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        png_init_io(png_ptr, fp);

        png_byte chunk_name[4] = {'t', 'E', 'S', 'T'};
        if (Size >= 8) {
          memcpy(chunk_name, Data + 4, 4);
        } else if (Size > 4) {
          memset(chunk_name, 'A', sizeof(chunk_name));
          memcpy(chunk_name, Data + 4, Size - 4);
        }

        png_uint_32 length = 0;
        if (Size >= 12) {
          length = (static_cast<png_uint_32>(Data[8]) << 24) |
                   (static_cast<png_uint_32>(Data[9]) << 16) |
                   (static_cast<png_uint_32>(Data[10]) << 8) |
                   static_cast<png_uint_32>(Data[11]);
        } else {
          length = u32;
        }

        png_write_chunk_start(png_ptr, chunk_name, length);
      }
      fclose(fp);
    }

    png_destroy_write_struct(&png_ptr, NULL);
  }

  return 0;
}