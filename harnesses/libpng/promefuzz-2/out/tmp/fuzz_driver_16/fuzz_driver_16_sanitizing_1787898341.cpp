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

static png_voidp CustomMalloc(png_structp png_ptr, png_alloc_size_t size) {
  (void)png_ptr;
  if (size == 0) return NULL;
  return malloc(size);
}

static void CustomFree(png_structp png_ptr, png_voidp ptr) {
  (void)png_ptr;
  free(ptr);
}

static void UserWarningFn(png_structp png_ptr, png_const_charp warning_message) {
  (void)png_ptr;
  (void)warning_message;
}

static void UserErrorFn(png_structp png_ptr, png_const_charp error_message) {
  (void)error_message;
  longjmp(png_jmpbuf(png_ptr), 1);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, UserErrorFn, UserWarningFn);
  if (png_ptr == NULL) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, CustomMalloc, CustomFree);
  } else {
    png_set_mem_fn(NULL, NULL, CustomMalloc, CustomFree);
  }

  size_t offset = 0;
  const size_t kMaxAllocs = 16;
  png_voidp ptrs[kMaxAllocs];
  size_t ptr_count = 0;
  memset(ptrs, 0, sizeof(ptrs));

  while (offset < Size && ptr_count < kMaxAllocs) {
    uint8_t op = Data[offset++];
    png_alloc_size_t alloc_size = 0;

    if (offset + sizeof(uint32_t) <= Size) {
      uint32_t v = 0;
      memcpy(&v, Data + offset, sizeof(v));
      offset += sizeof(v);
      alloc_size = (png_alloc_size_t)(v & 0xFFFF);
    } else if (offset < Size) {
      alloc_size = (png_alloc_size_t)Data[offset++];
    }

    switch (op % 6) {
      case 0: {
        png_voidp p = png_malloc(png_ptr, alloc_size);
        if (p != NULL) {
          ptrs[ptr_count++] = p;
          if (alloc_size > 0) {
            ((uint8_t*)p)[0] = (uint8_t)alloc_size;
          }
        }
        break;
      }
      case 1: {
        png_voidp p = png_calloc(png_ptr, alloc_size);
        if (p != NULL) {
          ptrs[ptr_count++] = p;
          if (alloc_size > 0) {
            volatile uint8_t b = ((uint8_t*)p)[0];
            (void)b;
          }
        }
        break;
      }
      case 2: {
        png_voidp p = png_malloc_default(png_ptr, alloc_size);
        if (p != NULL) {
          ptrs[ptr_count++] = p;
          if (alloc_size > 1) {
            ((uint8_t*)p)[alloc_size - 1] = 0;
          }
        }
        break;
      }
      case 3: {
        if (ptr_count > 0) {
          size_t idx = alloc_size % ptr_count;
          png_free(png_ptr, ptrs[idx]);
          ptrs[idx] = ptrs[ptr_count - 1];
          ptrs[ptr_count - 1] = NULL;
          --ptr_count;
        } else {
          png_free(png_ptr, NULL);
          png_free(NULL, NULL);
        }
        break;
      }
      case 4: {
        if (Data[0] & 2) {
          png_set_mem_fn(png_ptr, NULL, CustomMalloc, CustomFree);
        }
        break;
      }
      case 5: {
        png_voidp p = png_malloc(NULL, alloc_size);
        if (p != NULL) {
          free(p);
        }
        break;
      }
    }
  }

  for (size_t i = 0; i < ptr_count; ++i) {
    png_free(png_ptr, ptrs[i]);
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}