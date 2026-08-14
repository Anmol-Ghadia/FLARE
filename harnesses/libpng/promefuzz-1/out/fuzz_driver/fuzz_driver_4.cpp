// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_sig_cmp at png.c:74:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
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
  png_byte sigbuf[8];
  memset(sigbuf, 0, sizeof(sigbuf));

  size_t copy_len = Size < sizeof(sigbuf) ? Size : sizeof(sigbuf);
  if (copy_len > 0) {
    memcpy(sigbuf, Data, copy_len);
  }

  size_t start = 0;
  size_t num_to_check = 0;
  if (Size > 0) start = Data[0] % 10;
  if (Size > 1) num_to_check = Data[1] % 12;

  if (start <= 7) {
    size_t max_check = 8 - start;
    if (num_to_check > max_check) num_to_check = max_check;
    (void)png_sig_cmp(sigbuf, start, num_to_check);
  } else {
    (void)png_sig_cmp(sigbuf, start, num_to_check);
  }

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

  size_t bounded_check = copy_len;
  if (bounded_check > 8) bounded_check = 8;
  (void)png_sig_cmp(sigbuf, 0, bounded_check);

  return 0;
}