// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_free at pngmem.c:232:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
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

static void PNGAPI fuzz_error_fn(png_structp png_ptr, png_const_charp) {
  jmp_buf *jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb != NULL) longjmp(*jb, 1);
}

static void PNGAPI fuzz_warning_fn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI custom_malloc_fn(png_structp, png_alloc_size_t size) {
  if (size == 0) return NULL;
  return malloc(size);
}

static void PNGAPI custom_free_fn(png_structp, png_voidp ptr) {
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  jmp_buf jmpbuf;
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, &jmpbuf, fuzz_error_fn, fuzz_warning_fn);
  if (png_ptr == NULL) return 0;

  if (setjmp(jmpbuf) != 0) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
  } else if (Size > 0 && (Data[0] & 2)) {
    png_set_mem_fn(png_ptr, NULL, NULL, NULL);
  }

  size_t off = 0;
  unsigned iterations = 1;
  if (Size > 0) {
    iterations = 1 + (Data[off] % 16);
    off++;
  }

  for (unsigned i = 0; i < iterations; ++i) {
    png_alloc_size_t req = 0;
    if (off < Size) {
      uint8_t mode = Data[off++];
      switch (mode % 6) {
        case 0:
          req = 0;
          break;
        case 1:
          req = (off < Size) ? Data[off++] : 1;
          break;
        case 2:
          if (off + 1 < Size) {
            req = (static_cast<png_alloc_size_t>(Data[off]) << 8) |
                  static_cast<png_alloc_size_t>(Data[off + 1]);
            off += 2;
          } else {
            req = 256;
            off = Size;
          }
          break;
        case 3:
          req = 1024;
          break;
        case 4:
          req = 4096;
          break;
        default:
          req = static_cast<png_alloc_size_t>(~static_cast<size_t>(0));
          break;
      }
    }

    png_voidp p1 = png_malloc_default(png_ptr, req);
    if (p1 != NULL) {
      if (req > 0 && req <= 4096) {
        memset(p1, (off < Size) ? Data[off] : 0xA5, static_cast<size_t>(req));
      }
      png_free(png_ptr, p1);
    }

    png_voidp p2 = png_malloc(png_ptr, req);
    if (p2 != NULL) {
      if (req > 0 && req <= 4096) {
        memset(p2, (off < Size) ? (Data[off] ^ 0x5A) : 0x5A, static_cast<size_t>(req));
      }
      png_free(png_ptr, p2);
    }

    png_voidp p3 = png_calloc(png_ptr, req);
    if (p3 != NULL) {
      if (req > 0 && req <= 64) {
        volatile uint8_t acc = 0;
        uint8_t *bytes = reinterpret_cast<uint8_t*>(p3);
        for (size_t j = 0; j < static_cast<size_t>(req); ++j) acc |= bytes[j];
        (void)acc;
      }
      png_free(png_ptr, p3);
    }

    if (off < Size && (Data[off] & 1)) {
      off++;
      if (off < Size && (Data[off] & 1))
        png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
      else
        png_set_mem_fn(png_ptr, NULL, NULL, NULL);
      if (off < Size) off++;
    }
  }

  png_free(png_ptr, NULL);
  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}