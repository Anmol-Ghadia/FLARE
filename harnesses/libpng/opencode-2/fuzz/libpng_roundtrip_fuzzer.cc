/* libpng_roundtrip_fuzzer.cc
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

#if defined(PNG_SIMPLIFIED_READ_SUPPORTED) && defined(PNG_SIMPLIFIED_WRITE_SUPPORTED)

namespace {

static const size_t kPngHeaderSize = 8;
static const png_alloc_size_t kMaxImageBytes = 8u * 1024u * 1024u;
static const png_alloc_size_t kMaxEncodedBytes = 12u * 1024u * 1024u;

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < kPngHeaderSize || png_sig_cmp(data, 0, kPngHeaderSize) != 0) {
    return 0;
  }

  png_image input_image;
  memset(&input_image, 0, sizeof(input_image));
  input_image.version = PNG_IMAGE_VERSION;

  if (!png_image_begin_read_from_memory(&input_image, data, size)) {
    return 0;
  }

  input_image.format = (size & 1u) != 0 ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB;

  png_alloc_size_t decoded_size = PNG_IMAGE_SIZE(input_image);
  if (decoded_size == 0 || decoded_size > kMaxImageBytes) {
    png_image_free(&input_image);
    return 0;
  }

  png_bytep decoded = static_cast<png_bytep>(calloc(1, decoded_size));
  if (decoded == nullptr) {
    png_image_free(&input_image);
    return 0;
  }

  if (!png_image_finish_read(&input_image, nullptr, decoded, 0, nullptr)) {
    free(decoded);
    png_image_free(&input_image);
    return 0;
  }

  png_image output_image;
  memset(&output_image, 0, sizeof(output_image));
  output_image.version = PNG_IMAGE_VERSION;
  output_image.width = input_image.width;
  output_image.height = input_image.height;
  output_image.format = input_image.format;

  png_alloc_size_t encoded_size = PNG_IMAGE_PNG_SIZE_MAX(output_image);
  if (encoded_size == 0 || encoded_size > kMaxEncodedBytes) {
    free(decoded);
    png_image_free(&input_image);
    return 0;
  }

  png_bytep encoded = static_cast<png_bytep>(malloc(encoded_size));
  if (encoded == nullptr) {
    free(decoded);
    png_image_free(&input_image);
    return 0;
  }

  if (!png_image_write_to_memory(&output_image, encoded, &encoded_size, 0,
                                 decoded, 0, nullptr)) {
    png_image_free(&output_image);
    free(encoded);
    free(decoded);
    png_image_free(&input_image);
    return 0;
  }

  png_image_free(&output_image);
  free(encoded);
  free(decoded);
  png_image_free(&input_image);
  return 0;
}

#else

extern "C" int LLVMFuzzerTestOneInput(const uint8_t*, size_t) {
  return 0;
}

#endif
