// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_row at pngwrite.c:693:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
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
#include <vector>
#include <algorithm>

static void FuzzReadStatus(png_structp png_ptr, png_uint_32 row_num, int pass) {
  (void)row_num;
  (void)pass;
  (void)png_get_io_state(png_ptr);
}

static void FuzzWriteStatus(png_structp png_ptr, png_uint_32 row_num, int pass) {
  (void)row_num;
  (void)pass;
  (void)png_get_io_state(png_ptr);
}

static void FuzzErrorFn(png_structp png_ptr, png_const_charp error_msg) {
  (void)error_msg;
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzWarningFn(png_structp png_ptr, png_const_charp warning_msg) {
  (void)png_ptr;
  (void)warning_msg;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb+");
  if (!fp) return 0;

  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
    fflush(fp);
  }
  rewind(fp);

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, FuzzErrorFn, FuzzWarningFn);
  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, FuzzErrorFn, FuzzWarningFn);

  png_infop read_info = nullptr;
  png_infop write_info = nullptr;

  if (read_ptr) read_info = png_create_info_struct(read_ptr);
  if (write_ptr) write_info = png_create_info_struct(write_ptr);

  if (read_ptr) {
    if (setjmp(png_jmpbuf(read_ptr)) == 0) {
      png_init_io(read_ptr, fp);
      png_set_read_status_fn(read_ptr, FuzzReadStatus);
      (void)png_get_io_state(read_ptr);

      if (Size >= 8) {
        png_read_info(read_ptr, read_info);
        (void)png_get_io_state(read_ptr);
      }

      png_set_read_status_fn(read_ptr, nullptr);
      (void)png_get_io_state(read_ptr);
    }
  }

  rewind(fp);

  if (write_ptr && write_info) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      png_init_io(write_ptr, fp);
      png_set_write_status_fn(write_ptr, FuzzWriteStatus);
      (void)png_get_io_state(write_ptr);

      png_uint_32 width = Size > 0 ? (png_uint_32)(Data[0] % 8 + 1) : 1;
      png_uint_32 height = Size > 1 ? (png_uint_32)(Data[1] % 8 + 1) : 1;
      int color_type = (Size > 2 && (Data[2] & 1)) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY;
      int bit_depth = 8;

      png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                   PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
      png_write_info(write_ptr, write_info);
      (void)png_get_io_state(write_ptr);

      size_t channels = (color_type == PNG_COLOR_TYPE_RGB) ? 3u : 1u;
      size_t rowbytes = width * channels;
      std::vector<png_byte> rowbuf(rowbytes);
      for (size_t i = 0; i < rowbytes; ++i) {
        rowbuf[i] = (i < Size) ? Data[i] : 0;
      }

      if (Size > 3 && (Data[3] & 1)) {
        for (png_uint_32 y = 0; y < height; ++y) {
          for (size_t i = 0; i < rowbytes; ++i) {
            rowbuf[i] = (png_byte)((i + y < Size) ? Data[(i + y) % Size] : (i + y));
          }
          png_write_row(write_ptr, rowbuf.data());
          (void)png_get_io_state(write_ptr);
        }
      } else {
        std::vector<png_bytep> rows(height);
        std::vector<std::vector<png_byte>> storage(height, std::vector<png_byte>(rowbytes));
        for (png_uint_32 y = 0; y < height; ++y) {
          for (size_t i = 0; i < rowbytes; ++i) {
            storage[y][i] = (png_byte)((Size > 0) ? Data[(y * rowbytes + i) % Size] : (y + i));
          }
          rows[y] = storage[y].data();
        }

        png_uint_32 first_batch = height > 0 ? std::min<png_uint_32>(height, (Size > 4 ? (Data[4] % height) + 1 : 1)) : 0;
        if (first_batch > 0) {
          png_write_rows(write_ptr, rows.data(), first_batch);
          (void)png_get_io_state(write_ptr);
        }
        if (first_batch < height) {
          png_write_rows(write_ptr, rows.data() + first_batch, height - first_batch);
          (void)png_get_io_state(write_ptr);
        }
      }

      png_set_write_status_fn(write_ptr, nullptr);
      png_write_end(write_ptr, write_info);
      (void)png_get_io_state(write_ptr);
    }
  }

  if (read_ptr || read_info) {
    png_destroy_read_struct(read_ptr ? &read_ptr : nullptr,
                            read_info ? &read_info : nullptr, nullptr);
  }
  if (write_ptr || write_info) {
    png_destroy_write_struct(write_ptr ? &write_ptr : nullptr,
                             write_info ? &write_info : nullptr);
  }

  fclose(fp);
  return 0;
}