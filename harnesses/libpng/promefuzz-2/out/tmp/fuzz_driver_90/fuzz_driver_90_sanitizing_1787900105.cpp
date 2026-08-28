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

static void PNGCBAPI fuzz_error_fn(png_structp png_ptr, png_const_charp) {
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb != NULL) longjmp(*jb, 1);
}

static void PNGCBAPI fuzz_warning_fn(png_structp, png_const_charp) {
}

static png_voidp PNGCBAPI custom_malloc_fn(png_structp, png_alloc_size_t size) {
  if (size == 0) return NULL;
  return malloc(size);
}

static void PNGCBAPI custom_free_fn(png_structp, png_voidp ptr) {
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  jmp_buf jmp_env;
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, &jmp_env, fuzz_error_fn, fuzz_warning_fn);
  if (png_ptr == NULL) return 0;

  if (setjmp(jmp_env) != 0) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (Size >= 1 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
  } else {
    png_set_mem_fn(png_ptr, NULL, NULL, NULL);
  }

  size_t offsets[6] = {0, 1, 2, 4, 8, 16};
  for (size_t i = 0; i < 6; ++i) {
    size_t off = offsets[i];
    png_alloc_size_t alloc_size = 0;

    if (Size > off) {
      size_t remaining = Size - off;
      size_t take = remaining < sizeof(size_t) ? remaining : sizeof(size_t);
      for (size_t j = 0; j < take; ++j) {
        alloc_size = (alloc_size << 8) | Data[off + j];
      }
    }

    alloc_size &= 0xffff;

    if (i == 0) {
      png_voidp p = png_malloc_default(png_ptr, alloc_size);
      if (p != NULL) {
        if (alloc_size > 0) memset(p, 0xA5, alloc_size);
        png_free(png_ptr, p);
      }
    } else if (i == 1) {
      png_voidp p = png_malloc(png_ptr, alloc_size);
      if (p != NULL) {
        if (alloc_size > 0 && Size > 0) {
          size_t n = alloc_size < Size ? alloc_size : Size;
          memcpy(p, Data, n);
        }
        png_free(png_ptr, p);
      }
    } else if (i == 2) {
      png_voidp p = png_calloc(png_ptr, alloc_size);
      if (p != NULL) {
        png_free(png_ptr, p);
      }
    } else if (i == 3) {
      png_voidp p1 = png_malloc_default(png_ptr, alloc_size);
      png_voidp p2 = png_malloc(png_ptr, alloc_size / 2);
      if (p1 != NULL) png_free(png_ptr, p1);
      if (p2 != NULL) png_free(png_ptr, p2);
    } else if (i == 4) {
      if (Size >= 2 && (Data[1] & 1)) {
        png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
      } else {
        png_set_mem_fn(png_ptr, NULL, NULL, NULL);
      }
      png_voidp p = png_calloc(png_ptr, alloc_size);
      if (p != NULL) png_free(png_ptr, p);
    } else {
      png_free(png_ptr, NULL);
      png_voidp p = png_malloc(png_ptr, alloc_size);
      if (p != NULL) {
        png_free(png_ptr, p);
        png_free(png_ptr, p);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}