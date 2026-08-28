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

static void user_read_data(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
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

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void user_error_fn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void user_warning_fn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_set_add_alpha(NULL, 0, PNG_FILLER_AFTER);
  png_set_user_transform_info(NULL, NULL, 0, 0);
  png_set_expand(NULL);
  png_set_interlace_handling(NULL);
  png_set_gray_to_rgb(NULL);
  png_set_filler(NULL, 0, PNG_FILLER_BEFORE);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn);
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

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &state, user_read_data);

  int depth = 0;
  int channels = 0;
  void* user_transform_context = NULL;
  if (Size >= 2) {
    depth = Data[0];
    channels = Data[1];
  } else if (Size == 1) {
    depth = Data[0];
  }

  png_set_user_transform_info(png_ptr, user_transform_context, depth, channels);

  if (Size > 2) {
    if (Data[2] & 0x01) png_set_expand(png_ptr);
    if (Data[2] & 0x02) png_set_gray_to_rgb(png_ptr);
    if (Data[2] & 0x04) png_set_filler(png_ptr, 0x00U, PNG_FILLER_BEFORE);
    if (Data[2] & 0x08) png_set_filler(png_ptr, 0xFFU, PNG_FILLER_AFTER);
    if (Data[2] & 0x10) png_set_add_alpha(png_ptr, 0x00U, PNG_FILLER_BEFORE);
    if (Data[2] & 0x20) png_set_add_alpha(png_ptr, 0xFFFFU, PNG_FILLER_AFTER);
  }

  png_read_info(png_ptr, info_ptr);

  if (Size > 3 && (Data[3] & 0x01)) {
    (void)png_set_interlace_handling(png_ptr);
  }

  if (Size > 3) {
    if (Data[3] & 0x02) png_set_expand(png_ptr);
    if (Data[3] & 0x04) png_set_gray_to_rgb(png_ptr);
    if (Data[3] & 0x08) png_set_filler(png_ptr, 0xABU, PNG_FILLER_AFTER);
    if (Data[3] & 0x10) png_set_add_alpha(png_ptr, 0xCDU, PNG_FILLER_BEFORE);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes == 0 || height == 0) {
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  const png_uint_32 kMaxDim = 4096;
  const png_size_t kMaxRowbytes = 1u << 20;
  const png_uint_32 kMaxRows = 4096;

  if (width > kMaxDim || height > kMaxRows || rowbytes > kMaxRowbytes) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_bytep row = reinterpret_cast<png_bytep>(malloc(rowbytes));
  if (row == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    png_read_row(png_ptr, row, NULL);
  }

  png_read_end(png_ptr, end_info);

  free(row);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}