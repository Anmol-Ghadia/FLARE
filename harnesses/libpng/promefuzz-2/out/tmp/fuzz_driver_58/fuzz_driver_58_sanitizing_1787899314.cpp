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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end of buffer");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState state = {Data, Size, 0};

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorCallback, PngWarningCallback);
  if (png_ptr == NULL)
    return 0;

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

  png_color_8 sig_bit;
  memset(&sig_bit, 0, sizeof(sig_bit));
  if (Size > 0) sig_bit.red = Data[0];
  if (Size > 1) sig_bit.green = Data[1];
  if (Size > 2) sig_bit.blue = Data[2];
  if (Size > 3) sig_bit.gray = Data[3];
  if (Size > 4) sig_bit.alpha = Data[4];

  png_set_shift(png_ptr, &sig_bit);
  png_set_sBIT(png_ptr, info_ptr, &sig_bit);

  png_color_8p out_sig = NULL;
  (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);

  int transforms = 0;
  if (Size > 5) {
    uint32_t mask = Data[5];
#ifdef PNG_TRANSFORM_STRIP_16
    if (mask & 0x01) transforms |= PNG_TRANSFORM_STRIP_16;
#endif
#ifdef PNG_TRANSFORM_STRIP_ALPHA
    if (mask & 0x02) transforms |= PNG_TRANSFORM_STRIP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_PACKING
    if (mask & 0x04) transforms |= PNG_TRANSFORM_PACKING;
#endif
#ifdef PNG_TRANSFORM_PACKSWAP
    if (mask & 0x08) transforms |= PNG_TRANSFORM_PACKSWAP;
#endif
#ifdef PNG_TRANSFORM_EXPAND
    if (mask & 0x10) transforms |= PNG_TRANSFORM_EXPAND;
#endif
#ifdef PNG_TRANSFORM_INVERT_MONO
    if (mask & 0x20) transforms |= PNG_TRANSFORM_INVERT_MONO;
#endif
#ifdef PNG_TRANSFORM_SHIFT
    if (mask & 0x40) transforms |= PNG_TRANSFORM_SHIFT;
#endif
#ifdef PNG_TRANSFORM_BGR
    if (mask & 0x80) transforms |= PNG_TRANSFORM_BGR;
#endif
  }

  if (Size > 6) {
#ifdef PNG_TRANSFORM_SWAP_ALPHA
    if (Data[6] & 0x01) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
    if (Data[6] & 0x02) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
    if (Data[6] & 0x04) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_GRAY_TO_RGB
    if (Data[6] & 0x08) transforms |= PNG_TRANSFORM_GRAY_TO_RGB;
#endif
#ifdef PNG_TRANSFORM_EXPAND_16
    if (Data[6] & 0x10) transforms |= PNG_TRANSFORM_EXPAND_16;
#endif
#ifdef PNG_TRANSFORM_SCALE_16
    if (Data[6] & 0x20) transforms |= PNG_TRANSFORM_SCALE_16;
#endif
#endif

  if (Size > 7 && (Data[7] & 1)) {
    png_read_png(png_ptr, info_ptr, transforms, NULL);
    (void)png_get_rowbytes(png_ptr, info_ptr);
    (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);
  } else {
    png_read_info(png_ptr, info_ptr);
    png_read_update_info(png_ptr, info_ptr);
    (void)png_get_rowbytes(png_ptr, info_ptr);
    (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);

    if (Size > 7 && (Data[7] & 2)) {
      png_read_update_info(png_ptr, info_ptr);
    }

    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (height > 0 && rowbytes > 0 && height < 4096 && rowbytes < (1u << 20)) {
      png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
      if (row != NULL) {
        for (png_uint_32 y = 0; y < height; ++y) {
          png_read_row(png_ptr, row, NULL);
        }
        free(row);
      }
      png_read_end(png_ptr, end_info);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}