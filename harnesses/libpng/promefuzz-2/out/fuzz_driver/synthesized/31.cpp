// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_write_chunk_data at pngwutil.c:127:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_write_chunk_data at pngwutil.c:127:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_31(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb+");
  if (!fp) return 0;

  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
    fflush(fp);
  }
  rewind(fp);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  int flush_rows = 0;
  if (Size >= 4) {
    flush_rows = (int)((Data[0] << 24) | (Data[1] << 16) | (Data[2] << 8) | Data[3]);
  } else if (Size >= 1) {
    flush_rows = (int)Data[0] - 128;
  }
  png_set_flush(png_ptr, flush_rows);

  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  png_byte sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  fwrite(sig, 1, sizeof(sig), fp);

  png_byte ihdr_data[13];
  memset(ihdr_data, 0, sizeof(ihdr_data));
  ihdr_data[0] = 0;
  ihdr_data[1] = 0;
  ihdr_data[2] = 0;
  ihdr_data[3] = 1;
  ihdr_data[4] = 0;
  ihdr_data[5] = 0;
  ihdr_data[6] = 0;
  ihdr_data[7] = 1;
  ihdr_data[8] = 8;
  ihdr_data[9] = 2;
  ihdr_data[10] = 0;
  ihdr_data[11] = 0;
  ihdr_data[12] = 0;

  png_write_chunk_start(png_ptr, (png_const_bytep)"IHDR", 13);
  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);
  png_write_chunk_data(png_ptr, ihdr_data, 13);
  png_write_chunk_end(png_ptr);

  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  if (Size > 4) {
    size_t remaining = Size - 4;
    png_uint_32 chunk_len = (png_uint_32)(remaining > 1024 ? 1024 : remaining);
    png_byte chunk_name[5] = {'f', 'U', 'Z', 'Z', 0};

    if (Size >= 8) {
      chunk_name[0] = (png_byte)((Data[4] >= 'A' && Data[4] <= 'z') ? Data[4] : 'f');
      chunk_name[1] = (png_byte)((Data[5] >= 'A' && Data[5] <= 'z') ? Data[5] : 'U');
      chunk_name[2] = (png_byte)((Data[6] >= 'A' && Data[6] <= 'z') ? Data[6] : 'Z');
      chunk_name[3] = (png_byte)((Data[7] >= 'A' && Data[7] <= 'z') ? Data[7] : 'Z');
    }

    png_write_chunk_start(png_ptr, chunk_name, chunk_len);
    (void)png_get_io_state(png_ptr);
    (void)png_get_io_chunk_type(png_ptr);

    if (chunk_len > 0) {
      png_write_chunk_data(png_ptr, Data + 4, chunk_len);
    }
    png_write_chunk_end(png_ptr);

    (void)png_get_io_state(png_ptr);
    (void)png_get_io_chunk_type(png_ptr);
  }

  png_write_end(png_ptr, info_ptr);

  (void)png_get_io_state(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}