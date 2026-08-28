// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_longjmp at pngerror.c:757:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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

static void PNGAPI FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  if (png_ptr) {
    png_longjmp(png_ptr, 1);
  }
}

static void PNGAPI FuzzWarnFn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI FuzzMallocFn(png_structp, png_alloc_size_t size) {
  if (size == 0) return nullptr;
  return malloc((size_t)size);
}

static void PNGAPI FuzzFreeFn(png_structp, png_voidp ptr) {
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput_4(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data != nullptr && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_byte sigbuf[8] = {0};
  size_t copy_n = Size < sizeof(sigbuf) ? Size : sizeof(sigbuf);
  if (Data != nullptr && copy_n > 0) {
    memcpy(sigbuf, Data, copy_n);
  }

  size_t start = 0;
  size_t num_to_check = 8;
  if (Size > 0) {
    start = Data[0] % 10;
  }
  if (Size > 1) {
    num_to_check = Data[1] % 10;
  }
  (void)png_sig_cmp(sigbuf, start, num_to_check);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               FuzzErrorFn, FuzzWarnFn);
  png_infop info_ptr = nullptr;

  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      info_ptr = png_create_info_struct(png_ptr);
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  }

  png_structp png_ptr2 = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                FuzzErrorFn, FuzzWarnFn);
  png_infop info_ptr2 = nullptr;

  if (png_ptr2 != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr2)) == 0) {
      info_ptr2 = png_create_info_struct(png_ptr2);
    }
    png_destroy_read_struct(&png_ptr2, &info_ptr2, nullptr);
  }

  png_structp png_ptr3 = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                FuzzErrorFn, FuzzWarnFn);
  if (png_ptr3 != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr3)) == 0) {
      png_voidp mem_ptr = nullptr;
      if (Size > 2) {
        mem_ptr = (png_voidp)(uintptr_t)Data[2];
      }

      png_malloc_ptr malloc_fn = nullptr;
      png_free_ptr free_fn = nullptr;

      if (Size > 3 && (Data[3] & 1)) {
        malloc_fn = FuzzMallocFn;
      }
      if (Size > 4 && (Data[4] & 1)) {
        free_fn = FuzzFreeFn;
      }

      png_set_mem_fn(png_ptr3, mem_ptr, malloc_fn, free_fn);
    }
    png_destroy_read_struct(&png_ptr3, nullptr, nullptr);
  }

  return 0;
}