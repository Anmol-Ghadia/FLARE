// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_create_write_struct_2 at pngwrite.c:505:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct_2 at pngwrite.c:505:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_longjmp at pngerror.c:757:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
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

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include <setjmp.h>

struct FuzzMemCtx {
  const uint8_t* data;
  size_t size;
  size_t offset;
  size_t limit;
  size_t allocated;
};

static void PNGAPI FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  png_longjmp(png_ptr, 1);
}

static void PNGAPI FuzzWarnFn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI FuzzMallocFn(png_structp png_ptr, png_alloc_size_t alloc_size) {
  FuzzMemCtx* ctx =
      static_cast<FuzzMemCtx*>(png_get_mem_ptr(png_ptr));
  if (ctx == nullptr) return nullptr;

  size_t req = static_cast<size_t>(alloc_size);
  if (req == 0) req = 1;

  if (req > ctx->limit) return nullptr;
  if (ctx->allocated > ctx->limit - req) return nullptr;

  void* p = ::operator new(req, std::nothrow);
  if (p != nullptr) {
    ctx->allocated += req;
    if (ctx->data != nullptr && ctx->size != 0) {
      size_t remaining = (ctx->offset < ctx->size) ? (ctx->size - ctx->offset) : 0;
      size_t to_copy = remaining < req ? remaining : req;
      if (to_copy > 0) {
        memcpy(p, ctx->data + ctx->offset, to_copy);
        ctx->offset += to_copy;
      }
      if (to_copy < req) {
        memset(static_cast<uint8_t*>(p) + to_copy, 0, req - to_copy);
      }
    } else {
      memset(p, 0, req);
    }
  }
  return p;
}

static void PNGAPI FuzzFreeFn(png_structp, png_voidp ptr) {
  ::operator delete(ptr);
}

static void WriteDummyFile(const uint8_t* data, size_t size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) return;
  if (data != nullptr && size != 0) {
    fwrite(data, 1, size, fp);
  }
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  const char* valid_ver = PNG_LIBPNG_VER_STRING;
  const char* maybe_bad_ver = valid_ver;
  if (Size > 0 && (Data[0] & 1)) {
    maybe_bad_ver = "0.0.0";
  }

  FuzzMemCtx read_ctx;
  read_ctx.data = Data;
  read_ctx.size = Size;
  read_ctx.offset = 0;
  read_ctx.limit = (Size % 4096) + 1024;
  read_ctx.allocated = 0;

  FuzzMemCtx write_ctx;
  write_ctx.data = Data;
  write_ctx.size = Size;
  write_ctx.offset = Size > 1 ? 1 : 0;
  write_ctx.limit = ((Size >> 1) % 4096) + 1024;
  write_ctx.allocated = 0;

  png_structp read_ptr = png_create_read_struct_2(
      maybe_bad_ver,
      &read_ctx,
      FuzzErrorFn,
      FuzzWarnFn,
      &read_ctx,
      FuzzMallocFn,
      FuzzFreeFn);

  if (read_ptr != nullptr) {
    if (setjmp(png_jmpbuf(read_ptr)) == 0) {
      (void)png_get_header_version(read_ptr);
      (void)png_get_header_ver(read_ptr);
      (void)png_get_copyright(read_ptr);
      (void)png_get_libpng_ver(read_ptr);

      if (Size > 2 && (Data[1] & 1)) {
        (void)png_get_header_version(nullptr);
        (void)png_get_header_ver(nullptr);
        (void)png_get_copyright(nullptr);
      }
    }
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
  } else {
    (void)png_get_header_version(nullptr);
    (void)png_get_header_ver(nullptr);
    (void)png_get_copyright(nullptr);
  }

  png_structp write_ptr = png_create_write_struct_2(
      (Size > 0 && (Data[0] & 2)) ? maybe_bad_ver : valid_ver,
      &write_ctx,
      FuzzErrorFn,
      FuzzWarnFn,
      &write_ctx,
      FuzzMallocFn,
      FuzzFreeFn);

  if (write_ptr != nullptr) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      (void)png_get_header_version(write_ptr);
      (void)png_get_header_ver(write_ptr);
      (void)png_get_copyright(write_ptr);
      (void)png_get_libpng_ver(write_ptr);

      if (Size > 3 && (Data[2] & 1)) {
        (void)png_get_header_version(nullptr);
        (void)png_get_header_ver(nullptr);
        (void)png_get_copyright(nullptr);
      }
    }
    png_destroy_write_struct(&write_ptr, nullptr);
  }

  if (Size > 4) {
    png_structp alt_read = png_create_read_struct_2(
        valid_ver,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    if (alt_read != nullptr) {
      if (setjmp(png_jmpbuf(alt_read)) == 0) {
        (void)png_get_header_version(alt_read);
        (void)png_get_header_ver(alt_read);
        (void)png_get_copyright(alt_read);
        (void)png_get_libpng_ver(alt_read);
      }
      png_destroy_read_struct(&alt_read, nullptr, nullptr);
    }

    png_structp alt_write = png_create_write_struct_2(
        valid_ver,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    if (alt_write != nullptr) {
      if (setjmp(png_jmpbuf(alt_write)) == 0) {
        (void)png_get_header_version(alt_write);
        (void)png_get_header_ver(alt_write);
        (void)png_get_copyright(alt_write);
        (void)png_get_libpng_ver(alt_write);
      }
      png_destroy_write_struct(&alt_write, nullptr);
    }
  }

  return 0;
}