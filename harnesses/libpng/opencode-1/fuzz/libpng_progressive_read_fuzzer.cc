// libpng_progressive_read_fuzzer.cc
// This file is released under the libpng license.
// For conditions of distribution and use, see the disclaimer and license in png.h.

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <limits>

#include "png.h"

#if defined(PNG_PROGRESSIVE_READ_SUPPORTED)

namespace {

static const size_t kPngHeaderSize = 8;
static const size_t kMaxImageBytes = 8U * 1024U * 1024U;

struct ProgressiveState {
  png_bytep image;
  size_t rowbytes;
  png_uint_32 height;
};

#ifdef PNG_USER_MEM_SUPPORTED
void* LimitedMalloc(png_structp, png_alloc_size_t size) {
  if (size > kMaxImageBytes) {
    return NULL;
  }

  return malloc(size);
}

void LimitedFree(png_structp, png_voidp ptr) {
  free(ptr);
}
#endif

void InfoCallback(png_structp png_ptr, png_infop info_ptr) {
  ProgressiveState* state = static_cast<ProgressiveState*>(
      png_get_progressive_ptr(png_ptr));
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_type = 0;
  int compression_type = 0;
  int filter_type = 0;

  if (state == NULL ||
      !png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_type, &compression_type, &filter_type)) {
    png_error(png_ptr, "png_get_IHDR failed");
  }

#ifdef PNG_READ_EXPAND_SUPPORTED
  png_set_expand(png_ptr);
#endif
#ifdef PNG_READ_PACK_SUPPORTED
  png_set_packing(png_ptr);
#endif
#ifdef PNG_READ_GRAY_TO_RGB_SUPPORTED
  png_set_gray_to_rgb(png_ptr);
#endif
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#elif defined(PNG_READ_STRIP_16_TO_8_SUPPORTED)
  png_set_strip_16(png_ptr);
#endif
#ifdef PNG_READ_tRNS_SUPPORTED
  png_set_tRNS_to_alpha(png_ptr);
#endif
#ifdef PNG_READ_INTERLACING_SUPPORTED
  (void)png_set_interlace_handling(png_ptr);
#endif

  png_read_update_info(png_ptr, info_ptr);

  state->rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  state->height = height;

  const uint64_t total_bytes =
      static_cast<uint64_t>(state->rowbytes) * height;

  if (state->rowbytes == 0 || height == 0 || total_bytes > kMaxImageBytes ||
      total_bytes > std::numeric_limits<size_t>::max()) {
    png_error(png_ptr, "image too large");
  }

  state->image = static_cast<png_bytep>(malloc(static_cast<size_t>(total_bytes)));
  if (state->image == NULL) {
    png_error(png_ptr, "out of memory");
  }

  memset(state->image, 0, static_cast<size_t>(total_bytes));

  (void)bit_depth;
  (void)color_type;
  (void)interlace_type;
  (void)compression_type;
  (void)filter_type;
}

void RowCallback(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num,
    int) {
  ProgressiveState* state = static_cast<ProgressiveState*>(
      png_get_progressive_ptr(png_ptr));

  if (new_row == NULL || state == NULL || state->image == NULL ||
      row_num >= state->height) {
    return;
  }

  png_progressive_combine_row(
      png_ptr, state->image + static_cast<size_t>(row_num) * state->rowbytes,
      new_row);
}

void EndCallback(png_structp, png_infop) {}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < kPngHeaderSize || png_sig_cmp(data, 0, kPngHeaderSize) != 0) {
    return 0;
  }

  ProgressiveState state = {NULL, 0, 0};
  png_structp png_ptr = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

#ifdef PNG_USER_MEM_SUPPORTED
  png_set_mem_fn(png_ptr, NULL, LimitedMalloc, LimitedFree);
#endif

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    free(state.image);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);

#if defined(PNG_SET_OPTION_SUPPORTED) && defined(PNG_IGNORE_ADLER32)
  png_set_option(png_ptr, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
#endif

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
  png_set_user_limits(png_ptr, 2048U, 2048U);
  png_set_chunk_cache_max(png_ptr, 128U);
  png_set_chunk_malloc_max(png_ptr, kMaxImageBytes);
#endif

  png_set_progressive_read_fn(
      png_ptr, &state, InfoCallback, RowCallback, EndCallback);

  size_t offset = 0;
  while (offset < size) {
    size_t chunk_size = 1U + (data[offset] & 0x3fU);
    if (chunk_size > size - offset) {
      chunk_size = size - offset;
    }

    png_process_data(png_ptr, info_ptr,
        const_cast<png_bytep>(reinterpret_cast<const png_byte*>(data + offset)),
        chunk_size);
    offset += chunk_size;
  }

  free(state.image);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t*, size_t) {
  return 0;
}

#endif
