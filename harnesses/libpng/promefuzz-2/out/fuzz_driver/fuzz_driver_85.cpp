// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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

static uint32_t ReadU32(const uint8_t *data) {
  return (static_cast<uint32_t>(data[0]) << 24) |
         (static_cast<uint32_t>(data[1]) << 16) |
         (static_cast<uint32_t>(data[2]) << 8) |
         static_cast<uint32_t>(data[3]);
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
  for (int i = 0; i < 8 && offset + 4 <= Size; ++i) {
    png_alloc_size_t alloc_size =
        static_cast<png_alloc_size_t>(ReadU32(Data + offset) & 0xFFFF);
    offset += 4;

    switch ((i < static_cast<int>(Size)) ? (Data[i] % 3) : (i % 3)) {
      case 0: {
        png_voidp p = png_malloc(png_ptr, alloc_size);
        if (p != NULL) {
          if (alloc_size > 0) {
            memset(p, (offset < Size) ? Data[offset % Size] : 0, alloc_size);
          }
          png_free(png_ptr, p);
        }
        break;
      }

      case 1: {
        png_voidp p = png_malloc_default(png_ptr, alloc_size);
        if (p != NULL) {
          if (alloc_size > 0) {
            memset(p, 0xA5, alloc_size);
          }
          png_free(png_ptr, p);
        }
        break;
      }

      case 2: {
        png_voidp p = png_calloc(png_ptr, alloc_size);
        if (p != NULL) {
          png_free(png_ptr, p);
        }
        break;
      }
    }

    if (offset < Size && (Data[offset] & 1)) {
      if (Data[offset] & 2) {
        png_set_mem_fn(png_ptr, NULL, CustomMalloc, CustomFree);
      }
      offset++;
    }
  }

  png_free(png_ptr, NULL);
  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}