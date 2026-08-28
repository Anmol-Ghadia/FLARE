// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
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
#include <stdio.h>

static void PNGAPI fuzz_error_fn(png_structp png_ptr, png_const_charp) {
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
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

extern "C" int LLVMFuzzerTestOneInput_95(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  jmp_buf jmpbuf;
  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, &jmpbuf,
                             fuzz_error_fn, fuzz_warning_fn);
  if (png_ptr == NULL) return 0;

  if (setjmp(jmpbuf) == 0) {
    if (Size > 0) {
      png_voidp mem_ptr = const_cast<uint8_t*>(Data);
      if ((Data[0] & 1) != 0) {
        png_set_mem_fn(png_ptr, mem_ptr, custom_malloc_fn, custom_free_fn);
      } else {
        png_set_mem_fn(png_ptr, mem_ptr, NULL, NULL);
      }
    }

    size_t a = 0, b = 0, c = 0;
    if (Size >= 8) {
      memcpy(&a, Data, sizeof(size_t));
    } else {
      a = Size;
    }
    if (Size >= 16) {
      memcpy(&b, Data + 8, sizeof(size_t));
    } else {
      b = Size > 1 ? Data[Size / 2] : 1;
    }
    if (Size >= 24) {
      memcpy(&c, Data + 16, sizeof(size_t));
    } else {
      c = Size > 2 ? Data[Size - 1] : 2;
    }

    a %= 4096;
    b %= 4096;
    c %= 4096;

    png_voidp p1 = png_malloc_default(png_ptr, (png_alloc_size_t)a);
    if (p1 != NULL) {
      if (a > 0) memset(p1, 0xA5, a);
      png_free(png_ptr, p1);
    }

    png_voidp p2 = png_malloc(png_ptr, (png_alloc_size_t)b);
    if (p2 != NULL) {
      if (b > 0) memset(p2, 0x5A, b);
      png_free(png_ptr, p2);
    }

    png_voidp p3 = png_calloc(png_ptr, (png_alloc_size_t)c);
    if (p3 != NULL) {
      png_free(png_ptr, p3);
    }

    if (Size > 0 && (Data[0] & 2) != 0) {
      png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
      png_voidp p4 = png_malloc(png_ptr, (png_alloc_size_t)((a ^ b ^ c) % 4096));
      if (p4 != NULL) {
        png_free(png_ptr, p4);
      }
    }

    png_free(png_ptr, NULL);
    (void)png_malloc_default(NULL, 16);
    (void)png_malloc(NULL, 16);
    (void)png_calloc(NULL, 16);
    png_set_mem_fn(NULL, NULL, custom_malloc_fn, custom_free_fn);
    png_free(NULL, NULL);
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}