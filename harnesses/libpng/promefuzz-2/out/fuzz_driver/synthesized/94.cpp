// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
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

static void PngWriteFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput_94(const uint8_t *Data, size_t Size) {
  png_byte buf2[2] = {0, 0};
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte read4[4] = {0, 0, 0, 0};
  png_byte read2[2] = {0, 0};

  if (Size > 0) read2[0] = Data[0];
  if (Size > 1) read2[1] = Data[1];
  if (Size > 0) read4[0] = Data[0];
  if (Size > 1) read4[1] = Data[1];
  if (Size > 2) read4[2] = Data[2];
  if (Size > 3) read4[3] = Data[3];

  unsigned int v16_in = 0;
  if (Size >= 2) {
    v16_in = (static_cast<unsigned int>(Data[0]) << 8) |
             static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    v16_in = static_cast<unsigned int>(Data[0]);
  }

  png_uint_32 v32_in = 0;
  if (Size >= 4) {
    v32_in = (static_cast<png_uint_32>(Data[0]) << 24) |
             (static_cast<png_uint_32>(Data[1]) << 16) |
             (static_cast<png_uint_32>(Data[2]) << 8) |
             static_cast<png_uint_32>(Data[3]);
  } else {
    for (size_t i = 0; i < Size && i < 4; ++i) {
      v32_in = (v32_in << 8) | static_cast<png_uint_32>(Data[i]);
    }
  }

  png_save_uint_16(buf2, v16_in);
  (void)png_get_uint_16(buf2);
  (void)png_get_uint_16(read2);

  png_save_uint_32(buf4, v32_in);
  (void)png_get_uint_32(buf4);
  (void)png_get_uint_32(read4);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      FILE* fp = fopen("./dummy_file", "wb");
      if (fp != nullptr) {
        png_set_write_fn(png_ptr, fp, PngWriteFn, PngFlushFn);

        png_byte chunk_name[4] = {'t', 'E', 'S', 'T'};
        if (Size >= 4) {
          chunk_name[0] = Data[0];
          chunk_name[1] = Data[1];
          chunk_name[2] = Data[2];
          chunk_name[3] = Data[3];
        } else {
          for (size_t i = 0; i < Size && i < 4; ++i) {
            chunk_name[i] = Data[i];
          }
        }

        const png_bytep chunk_data =
            Size > 4 ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + 4))
                     : nullptr;
        size_t chunk_len = Size > 4 ? Size - 4 : 0;

        png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

        png_byte alt_name1[4] = {'I', 'D', 'A', 'T'};
        png_write_chunk(png_ptr, alt_name1, reinterpret_cast<png_const_bytep>(buf2), 2);

        png_byte alt_name2[4] = {'t', 'E', 'X', 't'};
        png_write_chunk(png_ptr, alt_name2, reinterpret_cast<png_const_bytep>(buf4), 4);

        fclose(fp);
      }
    }
    png_destroy_write_struct(&png_ptr, nullptr);
  }

  png_structp png_ptr2 = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                 PngErrorFn, PngWarningFn);
  if (png_ptr2 != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr2)) == 0) {
      (void)png_get_uint_31(png_ptr2, read4);
      (void)png_get_uint_31(png_ptr2, buf4);

      png_byte max31[4] = {0x7f, 0xff, 0xff, 0xff};
      (void)png_get_uint_31(png_ptr2, max31);

      png_byte over31[4] = {0xff, 0xff, 0xff, 0xff};
      (void)png_get_uint_31(png_ptr2, over31);
    }
    png_destroy_write_struct(&png_ptr2, nullptr);
  }

  return 0;
}