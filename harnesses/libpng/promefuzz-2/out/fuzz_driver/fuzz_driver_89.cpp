// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
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

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
  }

  size_t idx = 0;
  unsigned iterations = 1;
  if (Size > 0) {
    iterations = (Data[idx] % 16) + 1;
    idx++;
  }

  for (unsigned i = 0; i < iterations; ++i) {
    uint8_t op = 0;
    if (idx < Size) op = Data[idx++];

    png_alloc_size_t req = 0;
    if (idx < Size) {
      req = static_cast<png_alloc_size_t>(Data[idx++]);
    }
    if (idx < Size) {
      req = static_cast<png_alloc_size_t>((req << 8) | Data[idx++]);
    }
    if (idx < Size && (op & 0x20)) {
      req = static_cast<png_alloc_size_t>((req << 8) | Data[idx++]);
    }

    req %= 65536;

    png_voidp ptr = NULL;
    switch (op % 4) {
      case 0:
        ptr = png_malloc_default(png_ptr, req);
        break;
      case 1:
        ptr = png_malloc(png_ptr, req);
        break;
      case 2:
        ptr = png_calloc(png_ptr, req);
        break;
      case 3:
        if (op & 0x40) {
          png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
        }
        ptr = png_malloc(png_ptr, req);
        break;
    }

    if (ptr != NULL) {
      if (req > 0) {
        size_t fill = req < 32 ? static_cast<size_t>(req) : 32u;
        if (idx < Size) {
          memset(ptr, Data[idx], fill);
          idx++;
        } else {
          memset(ptr, 0xA5, fill);
        }
      }
      png_free(png_ptr, ptr);
      if (op & 0x80) {
        png_free(png_ptr, NULL);
      }
    } else {
      png_free(png_ptr, NULL);
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}