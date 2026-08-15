/* libpng_progressive_read_fuzzer.cc
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "png.h"

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED

namespace {

static const size_t kPngHeaderSize = 8;
static const png_alloc_size_t kMaxAllocation = 8u * 1024u * 1024u;
static const png_alloc_size_t kMaxImageBytes = 8u * 1024u * 1024u;

struct ProgressiveState {
  png_uint_32 height;
  png_alloc_size_t rowbytes;
  png_bytep image;
};

void* limited_malloc(png_structp, png_alloc_size_t size) {
  if (size > kMaxAllocation) {
    return nullptr;
  }

  return malloc(size);
}

void limited_free(png_structp, png_voidp ptr) {
  free(ptr);
}

void info_callback(png_structp png_ptr, png_infop info_ptr) {
  ProgressiveState* state =
      static_cast<ProgressiveState*>(png_get_progressive_ptr(png_ptr));
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_type = 0;
  int compression_type = 0;
  int filter_type = 0;

  if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                    &color_type, &interlace_type, &compression_type,
                    &filter_type)) {
    png_error(png_ptr, "png_get_IHDR failed");
  }

  (void)bit_depth;
  (void)color_type;
  (void)interlace_type;
  (void)compression_type;
  (void)filter_type;

  if (width == 0 || height == 0 ||
      height > (kMaxImageBytes / 4u) / width) {
    png_error(png_ptr, "image too large");
  }

#ifdef PNG_READ_GRAY_TO_RGB_SUPPORTED
  png_set_gray_to_rgb(png_ptr);
#endif
#ifdef PNG_READ_EXPAND_SUPPORTED
  png_set_expand(png_ptr);
  png_set_tRNS_to_alpha(png_ptr);
#endif
#if defined(PNG_READ_PACK_SUPPORTED) || defined(PNG_WRITE_PACK_SUPPORTED)
  png_set_packing(png_ptr);
#endif
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  png_set_scale_16(png_ptr);
#endif
#if defined(PNG_READ_INTERLACING_SUPPORTED) || defined(PNG_WRITE_INTERLACING_SUPPORTED)
  (void)png_set_interlace_handling(png_ptr);
#endif

  png_read_update_info(png_ptr, info_ptr);

  state->rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (state->rowbytes == 0 || state->rowbytes > kMaxImageBytes ||
      height > kMaxImageBytes / state->rowbytes) {
    png_error(png_ptr, "image too large");
  }

  state->height = height;
  free(state->image);
  state->image = static_cast<png_bytep>(
      calloc(1, state->rowbytes * state->height));
  if (state->image == nullptr) {
    png_error(png_ptr, "out of memory");
  }
}

void row_callback(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num,
                  int) {
  if (new_row == nullptr) {
    return;
  }

  ProgressiveState* state =
      static_cast<ProgressiveState*>(png_get_progressive_ptr(png_ptr));
  if (row_num >= state->height || state->rowbytes == 0 ||
      state->image == nullptr) {
    return;
  }

  png_progressive_combine_row(
      png_ptr, state->image + row_num * state->rowbytes, new_row);
}

void end_callback(png_structp, png_infop) {
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < kPngHeaderSize || png_sig_cmp(data, 0, kPngHeaderSize) != 0) {
    return 0;
  }

  ProgressiveState* state =
      static_cast<ProgressiveState*>(calloc(1, sizeof(*state)));
  if (state == nullptr) {
    return 0;
  }

#ifdef PNG_USER_MEM_SUPPORTED
  png_structp png_ptr = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr,
      limited_malloc, limited_free);
#else
  png_structp png_ptr = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
#endif
  if (png_ptr == nullptr) {
    free(state);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    free(state);
    return 0;
  }

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
  png_set_chunk_malloc_max(png_ptr, kMaxAllocation);
#endif
  png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
#if defined(PNG_SET_OPTION_SUPPORTED) && defined(PNG_IGNORE_ADLER32)
  png_set_option(png_ptr, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
#endif
  png_set_progressive_read_fn(png_ptr, state, info_callback, row_callback,
                              end_callback);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    free(state->image);
    free(state);
    return 0;
  }

  size_t offset = 0;
  while (offset < size) {
    size_t remaining = size - offset;
    size_t chunk_size = 1u + (data[offset] & 63u);
    if (chunk_size > remaining) {
      chunk_size = remaining;
    }

    png_process_data(png_ptr, info_ptr, const_cast<uint8_t*>(data + offset),
                     chunk_size);
    offset += chunk_size;

    png_uint_32 skip = png_process_data_skip(png_ptr);
    if (skip > size - offset) {
      break;
    }

    offset += skip;
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  free(state->image);
  free(state);
  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t*, size_t) {
  return 0;
}

#endif
