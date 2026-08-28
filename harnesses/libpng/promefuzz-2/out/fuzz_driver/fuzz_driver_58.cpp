// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_png at pngread.c:1049:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_shift at pngtrans.c:84:1 in png.h
// png_set_sBIT at pngset.c:638:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

struct BufferState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  BufferState* state =
      reinterpret_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (state == NULL || outBytes == NULL) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  if (state->offset > state->size ||
      byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read beyond end");
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
  BufferState state;
  state.data = Data;
  state.size = Size;
  state.offset = 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                             PngErrorCallback, PngWarningCallback);
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
    uint8_t mask = Data[5];
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
    uint8_t mask2 = Data[6];
#ifdef PNG_TRANSFORM_SWAP_ALPHA
    if (mask2 & 0x01) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
    if (mask2 & 0x02) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
    if (mask2 & 0x04) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_GRAY_TO_RGB
    if (mask2 & 0x08) transforms |= PNG_TRANSFORM_GRAY_TO_RGB;
#endif
#ifdef PNG_TRANSFORM_EXPAND_16
    if (mask2 & 0x10) transforms |= PNG_TRANSFORM_EXPAND_16;
#endif
#ifdef PNG_TRANSFORM_SCALE_16
    if (mask2 & 0x20) transforms |= PNG_TRANSFORM_SCALE_16;
#endif
  }

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