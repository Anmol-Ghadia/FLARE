#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

#define PNG_HEADER_SIZE 8U
#define MAX_IMAGE_PIXELS 100000000U
#define MAX_ALLOCATION_BYTES (8U * 1024U * 1024U)

struct progressive_state {
  png_bytep row;
  size_t rowbytes;
  png_uint_32 height;
};

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
static void *limited_malloc(png_structp png_ptr, png_alloc_size_t size) {
  (void)png_ptr;

  if (size > MAX_ALLOCATION_BYTES) {
    return NULL;
  }

  return malloc((size_t)size);
}

static void limited_free(png_structp png_ptr, png_voidp ptr) {
  (void)png_ptr;
  free(ptr);
}

static void progressive_info_callback(png_structp png_ptr, png_infop info_ptr) {
  struct progressive_state *state =
      (struct progressive_state *)png_get_progressive_ptr(png_ptr);
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_type = 0;
  int compression_type = 0;
  int filter_type = 0;

  if (state == NULL) {
    png_error(png_ptr, "missing state");
  }

  if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                    &interlace_type, &compression_type, &filter_type)) {
    png_error(png_ptr, "missing IHDR");
  }

  (void)bit_depth;
  (void)color_type;
  (void)interlace_type;
  (void)compression_type;
  (void)filter_type;

  if (width != 0 && height > MAX_IMAGE_PIXELS / width) {
    png_error(png_ptr, "image too large");
  }

#ifdef PNG_READ_EXPAND_SUPPORTED
  png_set_expand(png_ptr);
  png_set_tRNS_to_alpha(png_ptr);
#endif

#ifdef PNG_READ_GRAY_TO_RGB_SUPPORTED
  png_set_gray_to_rgb(png_ptr);
#endif

#ifdef PNG_READ_PACK_SUPPORTED
  png_set_packing(png_ptr);
#endif

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#elif defined(PNG_READ_STRIP_16_TO_8_SUPPORTED)
  png_set_strip_16(png_ptr);
#endif

#ifdef PNG_READ_INTERLACING_SUPPORTED
  (void)png_set_interlace_handling(png_ptr);
#endif

  png_read_update_info(png_ptr, info_ptr);

  state->rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  state->height = height;

  if (state->rowbytes > MAX_ALLOCATION_BYTES) {
    png_error(png_ptr, "row too large");
  }

  if (state->rowbytes != 0) {
    state->row = (png_bytep)png_malloc(png_ptr, state->rowbytes);
    memset(state->row, 0, state->rowbytes);
  }
}

static void progressive_row_callback(png_structp png_ptr, png_bytep new_row,
                                     png_uint_32 row_num, int pass) {
  struct progressive_state *state =
      (struct progressive_state *)png_get_progressive_ptr(png_ptr);

  (void)pass;

  if (state == NULL || state->row == NULL || new_row == NULL ||
      row_num >= state->height) {
    return;
  }

  png_progressive_combine_row(png_ptr, state->row, new_row);
}

static void progressive_end_callback(png_structp png_ptr, png_infop info_ptr) {
  (void)png_ptr;
  (void)info_ptr;
}
#endif

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  struct progressive_state state;
  size_t offset = 0;

  if (size < PNG_HEADER_SIZE || png_sig_cmp(data, 0, PNG_HEADER_SIZE) != 0) {
    return 0;
  }

  memset(&state, 0, sizeof(state));

  png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL,
                                     NULL, limited_malloc, limited_free);
  if (png_ptr == NULL) {
    return 0;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
#ifdef PNG_SET_OPTION_SUPPORTED
  (void)png_set_option(png_ptr, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
#endif

  png_set_progressive_read_fn(png_ptr, &state, progressive_info_callback,
                              progressive_row_callback,
                              progressive_end_callback);

  if (setjmp(png_jmpbuf(png_ptr)) != 0) {
    goto cleanup;
  }

  while (offset < size) {
    size_t chunk_size = 1U + (size_t)(data[offset] & 63U);

    if (chunk_size > size - offset) {
      chunk_size = size - offset;
    }

    png_process_data(png_ptr, info_ptr, (png_bytep)(data + offset), chunk_size);
    offset += chunk_size;
  }

cleanup:
  if (state.row != NULL) {
    png_free(png_ptr, state.row);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
#else
  (void)data;
  (void)size;
#endif

  return 0;
}
