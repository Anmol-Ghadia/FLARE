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
#include <stdlib.h>
#include <string.h>

static png_voidp FuzzMalloc(png_structp /*png_ptr*/, png_alloc_size_t size) {
  if (size == 0) size = 1;
  if (size > (1u << 20)) return NULL;
  return malloc((size_t)size);
}

static void FuzzFree(png_structp /*png_ptr*/, png_voidp ptr) {
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t start = 0;
  size_t num_to_check = 0;

  if (Size > 0) start = Data[0] % 10;
  if (Size > 1) num_to_check = Data[1] % 12;

  (void)png_sig_cmp(reinterpret_cast<png_const_bytep>(Data), start, num_to_check);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = NULL;

  if (png_ptr != NULL) {
    info_ptr = png_create_info_struct(png_ptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  png_infop info_after_destroy = png_create_info_struct(png_ptr);
  (void)info_after_destroy;

  png_destroy_read_struct(&png_ptr, &info_after_destroy, NULL);

  png_set_mem_fn(png_ptr, NULL, FuzzMalloc, FuzzFree);

  if (Size >= 8) {
    (void)png_sig_cmp(reinterpret_cast<png_const_bytep>(Data), 0, 8);
  } else if (Size > 0) {
    (void)png_sig_cmp(reinterpret_cast<png_const_bytep>(Data), 0, Size);
  }

  return 0;
}