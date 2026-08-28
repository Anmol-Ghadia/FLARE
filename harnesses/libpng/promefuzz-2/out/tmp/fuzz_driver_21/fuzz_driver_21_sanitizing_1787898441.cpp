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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb != NULL) longjmp(*jb, 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  jmp_buf jmp_env;
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, &jmp_env, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) return 0;

  if (setjmp(jmp_env) != 0) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  size_t off = 0;
  auto next_byte = [&](uint8_t defval) -> uint8_t {
    if (off < Size) return Data[off++];
    return defval;
  };

  auto next_size = [&](size_t defval) -> size_t {
    size_t v = 0;
    for (size_t i = 0; i < sizeof(size_t); ++i) {
      v = (v << 8) | next_byte(0);
    }
    return v ? v : defval;
  };

  png_info* info = NULL;
  size_t struct_size = next_size(sizeof(png_info));

  if ((next_byte(0) & 1) != 0) {
    size_t alloc_size = struct_size;
    if ((next_byte(0) & 1) != 0 && alloc_size > 0) {
      alloc_size = alloc_size / 2;
      if (alloc_size == 0) alloc_size = 1;
    } else if ((next_byte(0) & 1) != 0) {
      alloc_size += 16;
    }
    info = reinterpret_cast<png_info*>(malloc(alloc_size));
    if (info != NULL) {
      memset(info, next_byte(0), alloc_size);
    }
  }

  png_info_init_3(&info, struct_size);

  const int iterations = 1 + (Size > 0 ? (Data[0] % 8) : 0);
  for (int i = 0; i < iterations; ++i) {
    size_t req = next_size(1);
    req &= static_cast<size_t>(0xFFFF);

    png_voidp p1 = png_malloc(png_ptr, req);
    if (p1 != NULL) {
      size_t fill = req < 1024 ? req : 1024;
      if (fill > 0) {
        memset(p1, next_byte(0xAA), fill);
      }
      if ((next_byte(0) & 1) != 0) {
        png_free(png_ptr, p1);
      } else {
        png_free(png_ptr, p1);
      }
    }

    size_t req2 = next_size(1);
    req2 &= static_cast<size_t>(0xFFFF);
    png_voidp p2 = png_malloc_default(png_ptr, req2);
    if (p2 != NULL) {
      size_t fill2 = req2 < 1024 ? req2 : 1024;
      if (fill2 > 0) {
        memset(p2, next_byte(0x55), fill2);
      }
      png_free_default(png_ptr, p2);
    }

    if ((next_byte(0) & 1) != 0) {
      size_t new_struct_size = next_size(sizeof(png_info));
      png_info_init_3(&info, new_struct_size);
    }
  }

  if (info != NULL) {
    free(info);
    info = NULL;
  }

  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}