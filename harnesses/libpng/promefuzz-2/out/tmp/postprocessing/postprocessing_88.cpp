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

static png_uint_32 ReadU32(const uint8_t *data) {
  return (static_cast<png_uint_32>(data[0]) << 24) |
         (static_cast<png_uint_32>(data[1]) << 16) |
         (static_cast<png_uint_32>(data[2]) << 8) |
         (static_cast<png_uint_32>(data[3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, CustomMalloc, CustomFree);
  }

  size_t offset = 1;
  for (int i = 0; i < 8; ++i) {
    png_alloc_size_t alloc_size = 0;
    if (offset + 4 <= Size) {
      alloc_size = static_cast<png_alloc_size_t>(ReadU32(Data + offset));
      offset += 4;
    } else if (Size > 0) {
      alloc_size = static_cast<png_alloc_size_t>(Size);
    }

    alloc_size %= 1 << 20;  // cap allocations to 1MB

    png_voidp p1 = NULL;
    png_voidp p2 = NULL;
    png_voidp p3 = NULL;

    switch ((Size > offset ? Data[offset] : static_cast<uint8_t>(i)) % 4) {
      case 0:
        p1 = png_malloc_default(png_ptr, alloc_size);
        png_free(png_ptr, p1);
        break;

      case 1:
        p1 = png_malloc(png_ptr, alloc_size);
        if (p1 != NULL && alloc_size > 0) {
          memset(p1, (Size > offset ? Data[offset] : 0), alloc_size);
        }
        png_free(png_ptr, p1);
        break;

      case 2:
        p1 = png_calloc(png_ptr, alloc_size);
        png_free(png_ptr, p1);
        break;

      case 3:
        p1 = png_malloc_default(png_ptr, alloc_size);
        p2 = png_malloc(png_ptr, alloc_size / 2);
        p3 = png_calloc(png_ptr, alloc_size / 4);
        png_free(png_ptr, p3);
        png_free(png_ptr, p2);
        png_free(png_ptr, p1);
        break;
    }

    if (offset < Size) {
      ++offset;
    }

    if (offset < Size && (Data[offset] & 1)) {
      png_set_mem_fn(png_ptr, NULL, CustomMalloc, CustomFree);
    }
    if (offset < Size) {
      ++offset;
    }
  }

  png_free(png_ptr, NULL);
  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}