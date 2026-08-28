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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
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

  BufferState* st = static_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (Size < 8) {
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, PngReadCallback);
  png_read_info(png_ptr, info_ptr);

  png_read_update_info(png_ptr, info_ptr);

  size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) {
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_bytep row = static_cast<png_bytep>(png_malloc(png_ptr, rowbytes));
  png_bytep display_row = static_cast<png_bytep>(png_malloc(png_ptr, rowbytes));

  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  if (height > 1024) {
    height = 1024;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    if ((y & 1) == 0) {
      png_read_row(png_ptr, row, display_row);
    } else {
      png_read_row(png_ptr, row, NULL);
    }
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}