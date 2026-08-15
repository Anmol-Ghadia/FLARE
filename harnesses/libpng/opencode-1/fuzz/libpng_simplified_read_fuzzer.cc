// libpng_simplified_read_fuzzer.cc
// This file is released under the libpng license.
// For conditions of distribution and use, see the disclaimer and license in png.h.

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <limits>

#include "png.h"

#if defined(PNG_SIMPLIFIED_READ_SUPPORTED)

namespace {

static const size_t kPngHeaderSize = 8;
static const size_t kMaxImageBytes = 8U * 1024U * 1024U;

png_uint_32 SelectFormat(uint8_t selector) {
  static const png_uint_32 kFormats[] = {
    PNG_FORMAT_GRAY,
    PNG_FORMAT_GA,
    PNG_FORMAT_RGB,
    PNG_FORMAT_RGBA,
    PNG_FORMAT_LINEAR_Y,
    PNG_FORMAT_LINEAR_Y_ALPHA,
    PNG_FORMAT_LINEAR_RGB,
    PNG_FORMAT_LINEAR_RGB_ALPHA,
  };

  return kFormats[selector % (sizeof(kFormats) / sizeof(kFormats[0]))];
}

int GetImageBytes(const png_image* image, size_t* image_bytes) {
  const uint64_t bytes_per_row = static_cast<uint64_t>(image->width) *
      static_cast<uint64_t>(PNG_IMAGE_PIXEL_SIZE(image->format));
  const uint64_t total_bytes = bytes_per_row * image->height;

  if (image->width == 0 || image->height == 0 || bytes_per_row == 0 ||
      total_bytes > kMaxImageBytes ||
      total_bytes > std::numeric_limits<size_t>::max()) {
    return 0;
  }

  *image_bytes = static_cast<size_t>(total_bytes);
  return 1;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < kPngHeaderSize || png_sig_cmp(data, 0, kPngHeaderSize) != 0) {
    return 0;
  }

  png_image image;
  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;

  if (!png_image_begin_read_from_memory(&image, data, size)) {
    png_image_free(&image);
    return 0;
  }

  image.format = SelectFormat(data[size - 1]);

  if ((data[size - 1] & 1U) != 0) {
    image.flags |= PNG_IMAGE_FLAG_16BIT_sRGB;
  }

  size_t image_bytes = 0;
  if (!GetImageBytes(&image, &image_bytes)) {
    png_image_free(&image);
    return 0;
  }

  void* buffer = malloc(image_bytes);
  if (buffer == NULL) {
    png_image_free(&image);
    return 0;
  }

  (void)png_image_finish_read(&image, NULL, buffer, 0, NULL);

  free(buffer);
  png_image_free(&image);
  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t*, size_t) {
  return 0;
}

#endif
