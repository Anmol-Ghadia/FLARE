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
  if (size == 0) size = 1;
  return malloc(size);
}

static void PNGCBAPI custom_free_fn(png_structp, png_voidp ptr) {
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  jmp_buf jmp_main;
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, &jmp_main, fuzz_error_fn,
                             fuzz_warning_fn);
  if (png_ptr == NULL) return 0;

  if (setjmp(jmp_main) != 0) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
  } else if (Size > 0 && (Data[0] & 2)) {
    png_set_mem_fn(png_ptr, NULL, NULL, NULL);
  }

  size_t offset = 0;
  unsigned iterations = 1;
  if (Size > 0) {
    iterations = 1 + (Data[0] % 8);
    offset = 1;
  }

  for (unsigned i = 0; i < iterations; ++i) {
    png_alloc_size_t sz = 0;
    if (offset + sizeof(uint32_t) <= Size) {
      uint32_t v;
      memcpy(&v, Data + offset, sizeof(v));
      offset += sizeof(v);
      sz = static_cast<png_alloc_size_t>(v);
    } else if (offset < Size) {
      sz = static_cast<png_alloc_size_t>(Data[offset++]);
    }

    sz &= 0xffff;

    jmp_buf jmp_alloc;
    png_set_error_fn(png_ptr, &jmp_alloc, fuzz_error_fn, fuzz_warning_fn);

    if (setjmp(jmp_alloc) == 0) {
      int action = 0;
      if (offset < Size) action = Data[offset++] % 3;
      else action = i % 3;

      png_voidp p = NULL;
      if (action == 0) {
        p = png_malloc(png_ptr, sz);
      } else if (action == 1) {
        p = png_malloc_default(png_ptr, sz);
      } else {
        p = png_calloc(png_ptr, sz);
      }

      if (p != NULL) {
        if (sz > 0) {
          size_t touch = sz < 32 ? static_cast<size_t>(sz) : 32u;
          memset(p, static_cast<int>(i), touch);
        }
        png_free(png_ptr, p);
      }
    }

    if (offset < Size && (Data[offset] & 1)) {
      ++offset;
      if (offset < Size && (Data[offset - 1] & 2)) {
        png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
      } else {
        png_set_mem_fn(png_ptr, NULL, NULL, NULL);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}