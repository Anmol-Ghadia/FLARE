#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

#define PNG_HEADER_SIZE 8U
#define MAX_IMAGE_PIXELS 100000000U
#define MAX_OUTPUT_BYTES (8U * 1024U * 1024U)

#ifdef PNG_SIMPLIFIED_READ_SUPPORTED
static png_uint_32 pick_output_format(size_t size) {
  if ((size & 1U) != 0) {
    return PNG_FORMAT_LINEAR_RGB_ALPHA;
  }

  return PNG_FORMAT_RGBA;
}

static int compute_output_size(const png_image *image, size_t *output_size) {
  const size_t channels = PNG_IMAGE_PIXEL_CHANNELS(image->format);
  const size_t component_size = PNG_IMAGE_PIXEL_COMPONENT_SIZE(image->format);
  size_t row_stride = 0;
  size_t total_components = 0;

  if (channels == 0 || component_size == 0) {
    return 0;
  }

  if ((size_t)image->width > (size_t)-1 / channels) {
    return 0;
  }

  row_stride = (size_t)image->width * channels;

  if (row_stride != 0 && (size_t)image->height > (size_t)-1 / row_stride) {
    return 0;
  }

  total_components = (size_t)image->height * row_stride;

  if (total_components > (size_t)-1 / component_size) {
    return 0;
  }

  *output_size = total_components * component_size;
  return 1;
}
#endif

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
#ifdef PNG_SIMPLIFIED_READ_SUPPORTED
  png_image image;
  png_bytep buffer = NULL;
  size_t output_size = 0;

  if (size < PNG_HEADER_SIZE || png_sig_cmp(data, 0, PNG_HEADER_SIZE) != 0) {
    return 0;
  }

  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;

  if (!png_image_begin_read_from_memory(&image, data, size)) {
    png_image_free(&image);
    return 0;
  }

  image.flags |= PNG_IMAGE_FLAG_16BIT_sRGB;
  image.format = pick_output_format(size);

  if (image.width != 0 && image.height > MAX_IMAGE_PIXELS / image.width) {
    png_image_free(&image);
    return 0;
  }

  if (!compute_output_size(&image, &output_size) ||
      output_size == 0 || output_size > MAX_OUTPUT_BYTES) {
    png_image_free(&image);
    return 0;
  }

  buffer = (png_bytep)malloc(output_size);
  if (buffer != NULL) {
    (void)png_image_finish_read(&image, NULL, buffer, 0, NULL);
  }

  free(buffer);
  png_image_free(&image);
#else
  (void)data;
  (void)size;
#endif

  return 0;
}
