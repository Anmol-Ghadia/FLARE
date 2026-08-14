// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_write_struct_2 at pngwrite.c:505:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
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
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FuzzContext {
  const uint8_t* data;
  size_t size;
  size_t alloc_count;
  size_t free_count;
  size_t alloc_limit;
  uint32_t flags;
};

static png_voidp FuzzMalloc(png_structp png_ptr, png_alloc_size_t size) {
  (void)png_ptr;
  FuzzContext* ctx = reinterpret_cast<FuzzContext*>(png_get_mem_ptr(png_ptr));
  if (ctx != NULL) {
    ctx->alloc_count++;
    if (ctx->alloc_limit != 0 && size > ctx->alloc_limit) {
      return NULL;
    }
  }
  if (size == 0) size = 1;
  return malloc((size_t)size);
}

static void FuzzFree(png_structp png_ptr, png_voidp ptr) {
  FuzzContext* ctx = reinterpret_cast<FuzzContext*>(png_get_mem_ptr(png_ptr));
  if (ctx != NULL) {
    ctx->free_count++;
  }
  free(ptr);
}

static void FuzzError(png_structp png_ptr, png_const_charp msg) {
  (void)msg;
  FuzzContext* ctx = reinterpret_cast<FuzzContext*>(png_get_error_ptr(png_ptr));
  if (ctx != NULL) {
    ctx->flags ^= 0xE0U;
  }
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzWarn(png_structp png_ptr, png_const_charp msg) {
  (void)msg;
  FuzzContext* ctx = reinterpret_cast<FuzzContext*>(png_get_error_ptr(png_ptr));
  if (ctx != NULL) {
    ctx->flags ^= 0x0FU;
  }
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t off) {
  if (off + 4 > size) return 0;
  return (uint32_t(data[off]) << 24) |
         (uint32_t(data[off + 1]) << 16) |
         (uint32_t(data[off + 2]) << 8) |
         uint32_t(data[off + 3]);
}

extern "C" int LLVMFuzzerTestOneInput_9(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  FuzzContext ctx1;
  memset(&ctx1, 0, sizeof(ctx1));
  ctx1.data = Data;
  ctx1.size = Size;
  ctx1.alloc_limit = (Size >= 4) ? (ReadU32(Data, Size, 0) & 0xFFFFu) : 0;

  FuzzContext ctx2;
  memset(&ctx2, 0, sizeof(ctx2));
  ctx2.data = Data;
  ctx2.size = Size;
  ctx2.alloc_limit = (Size >= 8) ? (ReadU32(Data, Size, 4) & 0xFFFFu) : 1024;

  png_structp read_ptr = NULL;
  png_structp write_ptr = NULL;

  const char* ver1 = (Size > 0 && (Data[0] & 1)) ? PNG_LIBPNG_VER_STRING : "0.0.0";
  const char* ver2 = (Size > 1 && (Data[1] & 1)) ? PNG_LIBPNG_VER_STRING : "invalid";

  read_ptr = png_create_read_struct_2(
      ver1,
      &ctx1,
      FuzzError,
      FuzzWarn,
      &ctx1,
      FuzzMalloc,
      FuzzFree);

  write_ptr = png_create_write_struct_2(
      ver2,
      &ctx2,
      FuzzError,
      FuzzWarn,
      &ctx2,
      FuzzMalloc,
      FuzzFree);

  png_set_mem_fn(NULL, &ctx1, FuzzMalloc, FuzzFree);
  png_set_error_fn(NULL, &ctx1, FuzzError, FuzzWarn);
  (void)png_get_mem_ptr(NULL);
  (void)png_get_error_ptr(NULL);

  if (read_ptr != NULL) {
    if (setjmp(png_jmpbuf(read_ptr)) == 0) {
      (void)png_get_mem_ptr(read_ptr);
      (void)png_get_error_ptr(read_ptr);

      if (Size > 2 && (Data[2] & 1)) {
        png_set_mem_fn(read_ptr, &ctx2, FuzzMalloc, FuzzFree);
      } else {
        png_set_mem_fn(read_ptr, &ctx1, FuzzMalloc, FuzzFree);
      }

      if (Size > 3 && (Data[3] & 1)) {
        png_set_error_fn(read_ptr, &ctx2, FuzzError, FuzzWarn);
      } else {
        png_set_error_fn(read_ptr, &ctx1, FuzzError, FuzzWarn);
      }

      (void)png_get_mem_ptr(read_ptr);
      (void)png_get_error_ptr(read_ptr);

      if (Size > 4 && (Data[4] & 1)) {
        png_error(read_ptr, "fuzz_read_error");
      } else if (Size > 5 && (Data[5] & 1)) {
        png_warning(read_ptr, "fuzz_read_warning");
      }
    }
  }

  if (write_ptr != NULL) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      (void)png_get_mem_ptr(write_ptr);
      (void)png_get_error_ptr(write_ptr);

      if (Size > 6 && (Data[6] & 1)) {
        png_set_mem_fn(write_ptr, &ctx1, FuzzMalloc, FuzzFree);
      } else {
        png_set_mem_fn(write_ptr, &ctx2, FuzzMalloc, FuzzFree);
      }

      if (Size > 7 && (Data[7] & 1)) {
        png_set_error_fn(write_ptr, &ctx1, FuzzError, FuzzWarn);
      } else {
        png_set_error_fn(write_ptr, &ctx2, FuzzError, FuzzWarn);
      }

      (void)png_get_mem_ptr(write_ptr);
      (void)png_get_error_ptr(write_ptr);

      if (Size > 8 && (Data[8] & 1)) {
        png_error(write_ptr, "fuzz_write_error");
      } else if (Size > 9 && (Data[9] & 1)) {
        png_warning(write_ptr, "fuzz_write_warning");
      }
    }
  }

  if (read_ptr != NULL) {
    png_destroy_read_struct(&read_ptr, NULL, NULL);
  }

  if (write_ptr != NULL) {
    png_destroy_write_struct(&write_ptr, NULL);
  }

  return 0;
}