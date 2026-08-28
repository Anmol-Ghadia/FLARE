// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
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

static png_alloc_size_t read_size_field(const uint8_t* Data, size_t Size, size_t off) {
  png_alloc_size_t v = 0;
  if (off < Size) {
    size_t remaining = Size - off;
    size_t take = remaining < sizeof(size_t) ? remaining : sizeof(size_t);
    for (size_t j = 0; j < take; ++j) {
      v = (v << 8) | Data[off + j];
    }
  }
  return v & 0xffff;
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

  if (Size >= 1 && (Data[0] & 1))
    png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
  else
    png_set_mem_fn(png_ptr, NULL, NULL, NULL);

  const size_t offsets[] = {0, 1, 2, 4, 8, 16};
  for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
    png_alloc_size_t alloc_size = read_size_field(Data, Size, offsets[i]);

    switch (i) {
      case 0: {
        png_voidp p = png_malloc_default(png_ptr, alloc_size);
        if (p != NULL) {
          if (alloc_size > 0) memset(p, 0xA5, alloc_size);
          png_free(png_ptr, p);
        }
        break;
      }

      case 1: {
        png_voidp p = png_malloc(png_ptr, alloc_size);
        if (p != NULL) {
          if (alloc_size > 0 && Size > 0) {
            size_t n = alloc_size < Size ? alloc_size : Size;
            memcpy(p, Data, n);
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

      case 3: {
        png_voidp p1 = png_malloc_default(png_ptr, alloc_size);
        png_voidp p2 = png_malloc(png_ptr, alloc_size / 2);
        if (p1 != NULL) png_free(png_ptr, p1);
        if (p2 != NULL) png_free(png_ptr, p2);
        break;
      }

      case 4: {
        if (Size >= 2 && (Data[1] & 1))
          png_set_mem_fn(png_ptr, NULL, custom_malloc_fn, custom_free_fn);
        else
          png_set_mem_fn(png_ptr, NULL, NULL, NULL);

        png_voidp p = png_calloc(png_ptr, alloc_size);
        if (p != NULL) png_free(png_ptr, p);
        break;
      }

      case 5: {
        png_free(png_ptr, NULL);
        png_voidp p = png_malloc(png_ptr, alloc_size);
        if (p != NULL) {
          if (alloc_size > 0) {
            ((unsigned char*)p)[0] = (Size > 0) ? Data[0] : 0;
          }
          png_free(png_ptr, p);
        }
        break;
      }
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}