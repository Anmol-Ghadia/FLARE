// libpng_write_fuzzer.cc
// This file is released under the libpng license.
// For conditions of distribution and use, see the disclaimer and license in png.h.

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <limits>

#include "png.h"

#if defined(PNG_SIMPLIFIED_WRITE_SUPPORTED)

namespace {

static const size_t kConfigBytes = 4;
static const size_t kMaxImageBytes = 1U * 1024U * 1024U;
static const png_alloc_size_t kMaxEncodedBytes = 8U * 1024U * 1024U;

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

void FillImageBuffer(uint8_t* buffer, size_t buffer_size, const uint8_t* data,
    size_t size) {
  for (size_t i = 0; i < buffer_size; ++i) {
    buffer[i] = data[(kConfigBytes + i) % size];
  }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < kConfigBytes) {
    return 0;
  }

  png_image image;
  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;
  image.width = 1U + (data[0] & 0x7fU);
  image.height = 1U + (data[1] & 0x7fU);
  image.format = SelectFormat(data[2]);

  if ((data[3] & 0x01U) != 0) {
    image.flags |= PNG_IMAGE_FLAG_FAST;
  }

  if ((data[3] & 0x02U) != 0 &&
      (image.format & PNG_FORMAT_FLAG_COLOR) != 0) {
    image.flags |= PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB;
  }

  size_t image_bytes = 0;
  if (!GetImageBytes(&image, &image_bytes)) {
    return 0;
  }

  uint8_t* buffer = static_cast<uint8_t*>(malloc(image_bytes));
  if (buffer == NULL) {
    return 0;
  }

  FillImageBuffer(buffer, image_bytes, data, size);

  const int convert_to_8_bit = (data[3] & 0x04U) != 0;
  png_alloc_size_t output_size = 0;

  if (!png_image_write_to_memory(&image, NULL, &output_size, convert_to_8_bit,
          buffer, 0, NULL) ||
      output_size == 0 || output_size > kMaxEncodedBytes) {
    free(buffer);
    png_image_free(&image);
    return 0;
  }

  void* output = malloc(static_cast<size_t>(output_size));
  if (output == NULL) {
    free(buffer);
    png_image_free(&image);
    return 0;
  }

  (void)png_image_write_to_memory(&image, output, &output_size,
      convert_to_8_bit, buffer, 0, NULL);

  free(output);
  free(buffer);
  png_image_free(&image);
  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t*, size_t) {
  return 0;
}

#endif
