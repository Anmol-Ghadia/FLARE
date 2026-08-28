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
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL || outBytes == NULL) {
    png_error(png_ptr, "invalid io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* st = static_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp null_png = NULL;
  png_infop null_info = NULL;
  png_destroy_read_struct(&null_png, &null_info, NULL);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf)) == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(*png_jmpbuf(png_ptr)) == 0) {
    struct BufferState {
      const uint8_t* data;
      size_t size;
      size_t offset;
    } state = {Data, Size, 0};

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    png_read_info(png_ptr, info_ptr);

    png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_method = 0;
    int compression_method = 0, filter_method = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_method, &compression_method, &filter_method);

    png_set_gray_to_rgb(png_ptr);

    if ((Size & 1) == 0) {
      png_uint_32 w2 = 0, h2 = 0;
      png_get_IHDR(png_ptr, info_ptr, &w2, &h2, NULL, NULL, NULL, NULL, NULL);
    } else {
      png_get_IHDR(png_ptr, info_ptr, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  return 0;
}