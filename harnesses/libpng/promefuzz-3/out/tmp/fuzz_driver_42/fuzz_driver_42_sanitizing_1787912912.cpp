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

struct TransformState {
  uint8_t mode;
};

static void ReadUserTransform(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  (void)row_info;
  if (png_ptr == NULL || data == NULL) return;

  TransformState* state =
      static_cast<TransformState*>(png_get_user_transform_ptr(png_ptr));
  png_uint_32 row = png_get_current_row_number(png_ptr);

  if (state == NULL) return;

  if (state->mode & 1) {
    if (row_info != NULL && row_info->rowbytes > 0) {
      data[0] ^= static_cast<png_byte>(row & 0xFFu);
    }
  }

  if (state->mode & 2) {
    if (row_info != NULL && row_info->rowbytes > 1) {
      data[row_info->rowbytes - 1] ^= static_cast<png_byte>((row >> 8) & 0xFFu);
    }
  }
}

static void WriteUserTransform(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  (void)png_ptr;
  (void)row_info;
  (void)data;
}

static void PngReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == NULL || outBytes == NULL) return;
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    memset(outBytes, 0, byteCountToRead);
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = static_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  size_t remaining = st->size - st->offset;
  if (byteCountToRead > remaining) {
    if (remaining > 0) {
      memcpy(outBytes, st->data + st->offset, remaining);
      st->offset += remaining;
    }
    png_error(png_ptr, "read beyond end");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static void PngFlushData(png_structp) {
}

static void CleanupRows(png_bytep* rows, png_uint_32 height) {
  if (rows == NULL) return;
  for (png_uint_32 i = 0; i < height; ++i) {
    free(rows[i]);
  }
  free(rows);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    png_voidp p = png_get_user_transform_ptr(NULL);
    (void)p;
    png_uint_32 r = png_get_current_row_number(NULL);
    (void)r;
    png_read_row(NULL, NULL, NULL);
    png_set_read_user_transform_fn(NULL, ReadUserTransform);
    png_set_write_user_transform_fn(NULL, WriteUserTransform);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (Size == 0) return 0;

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader = {Data, Size, 0};

  TransformState transform_state;
  transform_state.mode = Data[0];

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (write_ptr != NULL) {
    png_set_write_user_transform_fn(write_ptr, WriteUserTransform);
    png_destroy_write_struct(&write_ptr, NULL);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  png_set_read_fn(png_ptr, &reader, PngReadData);
  png_set_read_user_transform_fn(png_ptr, ReadUserTransform);
  png_set_user_transform_info(png_ptr, &transform_state, 0, 0);

  if (transform_state.mode & 0x04) {
    png_set_interlace_handling(png_ptr);
  }

  if (transform_state.mode & 0x08) {
    png_set_expand(png_ptr);
  }

  if (transform_state.mode & 0x10) {
    png_set_gray_to_rgb(png_ptr);
  }

  if (transform_state.mode & 0x20) {
    png_set_strip_16(png_ptr);
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height == 0 || rowbytes == 0) {
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (height > 100000 || rowbytes > (1u << 24)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  png_bytep* rows = static_cast<png_bytep*>(calloc(height, sizeof(png_bytep)));
  if (rows == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  bool ok = true;
  for (png_uint_32 i = 0; i < height; ++i) {
    rows[i] = static_cast<png_bytep>(malloc(rowbytes));
    if (rows[i] == NULL) {
      ok = false;
      break;
    }
    memset(rows[i], 0, rowbytes);
  }

  if (!ok) {
    CleanupRows(rows, height);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  png_voidp user_ptr = png_get_user_transform_ptr(png_ptr);
  (void)user_ptr;
  png_uint_32 current_row = png_get_current_row_number(png_ptr);
  (void)current_row;

  if (transform_state.mode & 0x40) {
    for (png_uint_32 i = 0; i < height; ++i) {
      png_bytep display_row = (transform_state.mode & 0x80) ? rows[i] : NULL;
      png_read_row(png_ptr, rows[i], display_row);
      png_uint_32 cb_row = png_get_current_row_number(png_ptr);
      (void)cb_row;
    }
    png_read_end(png_ptr, info_ptr);
  } else {
    png_read_image(png_ptr, rows);
    png_read_end(png_ptr, info_ptr);
  }

  CleanupRows(rows, height);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}