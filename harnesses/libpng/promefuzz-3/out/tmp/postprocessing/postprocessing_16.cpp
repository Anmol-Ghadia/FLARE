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
#include <stdio.h>
#include <string.h>
#include <vector>

struct FuzzCtx {
  std::vector<unsigned char> bytes;
};

static void FuzzErrorFn(png_structp, png_const_charp) {}
static void FuzzWarningFn(png_structp, png_const_charp) {}

static png_voidp FuzzMallocFn(png_structp, png_alloc_size_t size) {
  if (size == 0) size = 1;
  return malloc(size);
}

static void FuzzFreeFn(png_structp, png_voidp ptr) {
  free(ptr);
}

static void FuzzReadFn(png_structp png_ptr, png_bytep outBytes, size_t byteCountToRead) {
  FuzzCtx* ctx = reinterpret_cast<FuzzCtx*>(png_get_io_ptr(png_ptr));
  if (ctx == nullptr || outBytes == nullptr) return;

  size_t n = byteCountToRead;
  if (n > ctx->bytes.size()) n = ctx->bytes.size();
  if (n > 0) memcpy(outBytes, ctx->bytes.data(), n);
  if (byteCountToRead > n) memset(outBytes + n, 0, byteCountToRead - n);
}

static void FuzzWriteFn(png_structp, png_bytep, size_t) {}
static void FuzzFlushFn(png_structp) {}

static int FuzzUserChunkFn(png_structp, png_unknown_chunkp) {
  return PNG_HANDLE_CHUNK_AS_DEFAULT;
}

static void FuzzUserTransformFn(png_structp, png_row_infop, png_bytep) {}

static void FuzzInfoFn(png_structp, png_infop) {}
static void FuzzRowFn(png_structp, png_bytep, png_uint_32, int) {}
static void FuzzEndFn(png_structp, png_infop) {}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzCtx err_ctx;
  FuzzCtx mem_ctx;
  FuzzCtx io_ctx;
  FuzzCtx chunk_ctx;
  FuzzCtx transform_ctx;
  FuzzCtx progressive_ctx;

  if (Data != nullptr && Size > 0) {
    io_ctx.bytes.assign(Data, Data + Size);
    err_ctx.bytes.assign(Data, Data + (Size > 16 ? 16 : Size));
    mem_ctx.bytes.assign(Data, Data + (Size > 32 ? 32 : Size));
    chunk_ctx.bytes.assign(Data, Data + (Size > 48 ? 48 : Size));
    transform_ctx.bytes.assign(Data, Data + (Size > 64 ? 64 : Size));
    progressive_ctx.bytes.assign(Data, Data + (Size > 80 ? 80 : Size));
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  (void)png_get_user_chunk_ptr(nullptr);
  (void)png_get_user_transform_ptr(nullptr);
  (void)png_get_io_ptr(nullptr);
  (void)png_get_progressive_ptr(nullptr);
  (void)png_get_error_ptr(nullptr);
  (void)png_get_mem_ptr(nullptr);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, &err_ctx,
                                               FuzzErrorFn, FuzzWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_mem_fn(png_ptr, &mem_ctx, FuzzMallocFn, FuzzFreeFn);
  png_set_read_fn(png_ptr, &io_ctx, FuzzReadFn);
  png_set_read_user_chunk_fn(png_ptr, &chunk_ctx, FuzzUserChunkFn);
  png_set_read_user_transform_fn(png_ptr, FuzzUserTransformFn);
  png_set_user_transform_info(png_ptr, &transform_ctx, 8, 1);
  png_set_progressive_read_fn(png_ptr, &progressive_ctx, FuzzInfoFn, FuzzRowFn, FuzzEndFn);

  png_voidp p1 = png_get_error_ptr(png_ptr);
  png_voidp p2 = png_get_mem_ptr(png_ptr);
  png_voidp p3 = png_get_io_ptr(png_ptr);
  png_voidp p4 = png_get_user_chunk_ptr(png_ptr);
  png_voidp p5 = png_get_user_transform_ptr(png_ptr);
  png_voidp p6 = png_get_progressive_ptr(png_ptr);

  volatile uintptr_t sink = 0;
  sink ^= reinterpret_cast<uintptr_t>(p1);
  sink ^= reinterpret_cast<uintptr_t>(p2);
  sink ^= reinterpret_cast<uintptr_t>(p3);
  sink ^= reinterpret_cast<uintptr_t>(p4);
  sink ^= reinterpret_cast<uintptr_t>(p5);
  sink ^= reinterpret_cast<uintptr_t>(p6);

  if (Size > 0) {
    size_t split1 = Data[0] % (Size + 1);
    png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(Data),
                     static_cast<png_size_t>(split1));
    if (split1 < Size) {
      png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(Data + split1),
                       static_cast<png_size_t>(Size - split1));
    }
  } else {
    png_process_data(png_ptr, info_ptr, nullptr, 0);
  }

  p1 = png_get_error_ptr(png_ptr);
  p2 = png_get_mem_ptr(png_ptr);
  p3 = png_get_io_ptr(png_ptr);
  p4 = png_get_user_chunk_ptr(png_ptr);
  p5 = png_get_user_transform_ptr(png_ptr);
  p6 = png_get_progressive_ptr(png_ptr);

  sink ^= reinterpret_cast<uintptr_t>(p1);
  sink ^= reinterpret_cast<uintptr_t>(p2);
  sink ^= reinterpret_cast<uintptr_t>(p3);
  sink ^= reinterpret_cast<uintptr_t>(p4);
  sink ^= reinterpret_cast<uintptr_t>(p5);
  sink ^= reinterpret_cast<uintptr_t>(p6);

  png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &err_ctx,
                                                  FuzzErrorFn, FuzzWarningFn);
  if (write_ptr != nullptr) {
    png_infop write_info = png_create_info_struct(write_ptr);
    if (write_info != nullptr) {
      if (!setjmp(png_jmpbuf(write_ptr))) {
        png_set_write_fn(write_ptr, &io_ctx, FuzzWriteFn, FuzzFlushFn);
        (void)png_get_error_ptr(write_ptr);
        (void)png_get_io_ptr(write_ptr);
        (void)png_get_user_chunk_ptr(write_ptr);
        (void)png_get_user_transform_ptr(write_ptr);
        (void)png_get_progressive_ptr(write_ptr);
        (void)png_get_mem_ptr(write_ptr);
      }
      png_destroy_write_struct(&write_ptr, &write_info);
    } else {
      png_destroy_write_struct(&write_ptr, nullptr);
    }
  }

  (void)sink;
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}