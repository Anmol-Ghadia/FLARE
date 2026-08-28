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
#include <setjmp.h>
#include <stdio.h>

struct FuzzState {
  size_t alloc_limit;
  int fail_after;
  int alloc_count;
};

static png_voidp FuzzMalloc(png_structp png_ptr, png_alloc_size_t size) {
  FuzzState* state =
      reinterpret_cast<FuzzState*>(png_get_mem_ptr(png_ptr));

  if (state != NULL) {
    if (state->fail_after >= 0 && state->alloc_count >= state->fail_after)
      return NULL;
    state->alloc_count++;
    if (size > state->alloc_limit)
      return NULL;
  }

  if (size == 0)
    size = 1;

  return malloc((size_t)size);
}

static void FuzzFree(png_structp, png_voidp ptr) {
  free(ptr);
}

static png_uint_32 ReadU32(const uint8_t* data) {
  return ((png_uint_32)data[0] << 24) |
         ((png_uint_32)data[1] << 16) |
         ((png_uint_32)data[2] << 8) |
         (png_uint_32)data[3];
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0)
      fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  if (Size < 9)
    return 0;

  size_t off = 0;
  uint8_t mode = Data[off++];
  png_alloc_size_t size1 = (png_alloc_size_t)ReadU32(Data + off);
  off += 4;
  png_alloc_size_t size2 = (png_alloc_size_t)ReadU32(Data + off);
  off += 4;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
    return 0;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  FuzzState state;
  state.alloc_limit = (Size > off) ? (size_t)Data[off++] * 1024u : 4096u;
  state.fail_after = (Size > off) ? (int)(Data[off++] % 8) - 1 : -1;
  state.alloc_count = 0;

  if (mode & 1) {
    png_set_mem_fn(png_ptr, &state, FuzzMalloc, FuzzFree);
  }

  png_voidp p1 = NULL;
  png_voidp p2 = NULL;
  png_voidp p3 = NULL;

  switch (mode % 6) {
    case 0:
      p1 = png_malloc(png_ptr, size1);
      if (p1 != NULL)
        memset(p1, 0xA5, (size_t)(size1 == 0 ? 1 : size1));
      png_free(png_ptr, p1);

      p2 = png_calloc(png_ptr, size2);
      png_free(png_ptr, p2);
      break;

    case 1:
      p1 = png_calloc(png_ptr, size1);
      p2 = png_malloc(png_ptr, size2);
      png_free(png_ptr, p2);
      png_free(png_ptr, p1);
      break;

    case 2:
      p1 = png_malloc(png_ptr, size1);
      if ((mode & 2) && p1 != NULL) {
        png_set_mem_fn(png_ptr, &state, FuzzMalloc, FuzzFree);
      }
      png_free(png_ptr, p1);
      p2 = png_calloc(png_ptr, size2);
      png_free(png_ptr, p2);
      break;

    case 3:
      p1 = png_malloc_default(png_ptr, size1);
      png_free(png_ptr, p1);
      p2 = png_calloc(png_ptr, size2);
      png_free(png_ptr, p2);
      break;

    case 4:
      p1 = png_calloc(png_ptr, size1);
      p2 = png_malloc_default(png_ptr, size2);
      png_free(png_ptr, p1);
      png_free(png_ptr, p2);
      break;

    case 5:
      p1 = png_malloc(png_ptr, size1);
      p2 = png_calloc(png_ptr, size2);
      p3 = png_malloc_default(png_ptr, (png_alloc_size_t)((size1 ^ size2) & 0xffff));
      png_free(png_ptr, p3);
      png_free(png_ptr, p2);
      png_free(png_ptr, p1);
      break;
  }

  if (mode & 4) {
    png_free(NULL, NULL);
    (void)png_malloc(NULL, size1);
    (void)png_calloc(NULL, size2);
    (void)png_malloc_default(NULL, size1);
    png_set_mem_fn(NULL, &state, FuzzMalloc, FuzzFree);
  }

  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}