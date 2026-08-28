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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* st = reinterpret_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static png_fixed_point MakeFixedPoint(const uint8_t* data, size_t size, size_t off, png_fixed_point fallback) {
  if (off + 4 > size) return fallback;
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               (static_cast<uint32_t>(data[off + 3]));
  return static_cast<png_fixed_point>(v);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == NULL || Size == 0) return 0;

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarnFn);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (info_ptr == NULL || end_info == NULL) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : NULL, end_info ? &end_info : NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } st = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &st, ReadDataFn);

  png_color_16 bg;
  memset(&bg, 0, sizeof(bg));
  if (Size > 0) bg.index = Data[0];
  if (Size > 2) bg.red = static_cast<png_uint_16>((static_cast<unsigned>(Data[1]) << 8) | Data[2]);
  if (Size > 4) bg.green = static_cast<png_uint_16>((static_cast<unsigned>(Data[3]) << 8) | Data[4]);
  if (Size > 6) bg.blue = static_cast<png_uint_16>((static_cast<unsigned>(Data[5]) << 8) | Data[6]);
  if (Size > 8) bg.gray = static_cast<png_uint_16>((static_cast<unsigned>(Data[7]) << 8) | Data[8]);

  int gamma_code_choices[] = {
    PNG_BACKGROUND_GAMMA_UNKNOWN,
    PNG_BACKGROUND_GAMMA_SCREEN,
    PNG_BACKGROUND_GAMMA_FILE,
    PNG_BACKGROUND_GAMMA_UNIQUE,
    -1,
    4
  };
  int gamma_code = gamma_code_choices[Data[0] % (sizeof(gamma_code_choices) / sizeof(gamma_code_choices[0]))];
  int need_expand = (Size > 1) ? (Data[1] % 3) : 0;

  png_fixed_point screen_gamma = MakeFixedPoint(Data, Size, 9, 100000);
  png_fixed_point file_gamma = MakeFixedPoint(Data, Size, 13, 100000);
  png_fixed_point background_gamma_fixed = MakeFixedPoint(Data, Size, 17, 100000);
  double background_gamma = 1.0;
  if (Size > 21) {
    background_gamma = ((Data[21] % 50) + 1) / 10.0;
  }

  if (Data[0] & 1) png_set_gray_to_rgb(png_ptr);
  if (Data[0] & 2) png_set_scale_16(png_ptr);
  if (Data[0] & 4) png_set_tRNS_to_alpha(png_ptr);
  if (Data[0] & 8) png_set_gamma_fixed(png_ptr, screen_gamma, file_gamma);
  if (Data[0] & 16) png_set_background_fixed(png_ptr, &bg, gamma_code, need_expand, background_gamma_fixed);

  png_read_info(png_ptr, info_ptr);

  if (Data[0] & 32) png_set_background(png_ptr, &bg, gamma_code, need_expand, background_gamma);
  if (Data[0] & 64) png_set_gray_to_rgb(png_ptr);
  if (Data[0] & 128) png_set_scale_16(png_ptr);

  png_set_tRNS_to_alpha(png_ptr);
  png_set_gamma_fixed(png_ptr,
                      screen_gamma > 0 ? screen_gamma : 100000,
                      file_gamma > 0 ? file_gamma : 100000);

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes > 0 && width > 0 && height > 0) {
    png_uint_32 max_rows = height;
    if (max_rows > 64) max_rows = 64;

    png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
    if (row != NULL) {
      for (png_uint_32 y = 0; y < max_rows; ++y) {
        png_read_row(png_ptr, row, NULL);
      }
      free(row);
    }
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}