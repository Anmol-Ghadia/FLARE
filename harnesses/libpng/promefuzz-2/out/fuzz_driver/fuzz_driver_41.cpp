// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <setjmp.h>

struct FuzzMemCtx {
  size_t alloc_limit;
  size_t total_allocated;
  uint8_t pattern;
};

static void PNGAPI FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PNGAPI FuzzWarnFn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI FuzzMallocFn(png_structp, png_alloc_size_t size) {
  void* p = std::malloc(static_cast<size_t>(size));
  if (p != nullptr) {
    std::memset(p, 0xA5, static_cast<size_t>(size));
  }
  return p;
}

static void PNGAPI FuzzFreeFn(png_structp, png_voidp ptr) {
  std::free(ptr);
}

static png_voidp PNGAPI LimitedMallocFn(png_structp png_ptr, png_alloc_size_t size) {
  FuzzMemCtx* ctx = static_cast<FuzzMemCtx*>(png_get_mem_ptr(png_ptr));
  if (ctx == nullptr) {
    return std::malloc(static_cast<size_t>(size));
  }

  size_t req = static_cast<size_t>(size);
  if (req > ctx->alloc_limit || ctx->total_allocated > ctx->alloc_limit - req) {
    return nullptr;
  }

  void* p = std::malloc(req);
  if (p != nullptr) {
    ctx->total_allocated += req;
    std::memset(p, ctx->pattern, req);
  }
  return p;
}

static void PNGAPI LimitedFreeFn(png_structp, png_voidp ptr) {
  std::free(ptr);
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  FuzzMemCtx ctx;
  ctx.alloc_limit = Size > 0 ? (Size * 4u + 64u) : 64u;
  ctx.total_allocated = 0;
  ctx.pattern = Size > 0 ? Data[0] : 0x5A;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, &ctx, FuzzErrorFn, FuzzWarnFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) != 0) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  (void)png_get_mem_ptr(nullptr);
  (void)png_get_mem_ptr(png_ptr);

  png_set_mem_fn(nullptr, &ctx, LimitedMallocFn, LimitedFreeFn);

  if (Size > 0) {
    switch (Data[0] % 3) {
      case 0:
        png_set_mem_fn(png_ptr, &ctx, LimitedMallocFn, LimitedFreeFn);
        break;
      case 1:
        png_set_mem_fn(png_ptr, &ctx, FuzzMallocFn, FuzzFreeFn);
        break;
      default:
        png_set_mem_fn(png_ptr, nullptr, FuzzMallocFn, FuzzFreeFn);
        break;
    }
  }

  (void)png_get_mem_ptr(png_ptr);

  const size_t kMaxAllocs = 8;
  png_voidp ptrs[kMaxAllocs];
  for (size_t i = 0; i < kMaxAllocs; ++i) ptrs[i] = nullptr;

  for (size_t i = 0; i < kMaxAllocs; ++i) {
    size_t off = 1 + i * 4;
    png_alloc_size_t req = static_cast<png_alloc_size_t>(ReadU32(Data, Size, off) & 0xFFFF);

    if (i == 0 && Size > 1) {
      req = static_cast<png_alloc_size_t>(Data[1]);
    }

    if ((Size > off) && (Data[off] & 1)) {
      ptrs[i] = png_malloc_default(png_ptr, req);
    } else {
      ptrs[i] = png_malloc(png_ptr, req);
    }

    if (ptrs[i] != nullptr && req > 0) {
      std::memset(ptrs[i], static_cast<int>((off < Size) ? Data[off] : 0xCC),
                  static_cast<size_t>(req));
    }

    if ((Size > off) && (Data[off] & 2)) {
      png_free(png_ptr, ptrs[i]);
      ptrs[i] = nullptr;
    }

    if ((Size > off) && (Data[off] & 4)) {
      (void)png_get_mem_ptr(png_ptr);
    }

    if ((Size > off) && (Data[off] & 8)) {
      if (Data[off] & 16) {
        png_set_mem_fn(png_ptr, &ctx, LimitedMallocFn, LimitedFreeFn);
      } else {
        png_set_mem_fn(png_ptr, &ctx, FuzzMallocFn, FuzzFreeFn);
      }
    }
  }

  for (size_t i = 0; i < kMaxAllocs; ++i) {
    png_free(png_ptr, ptrs[i]);
    ptrs[i] = nullptr;
  }

  png_free(nullptr, nullptr);
  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}