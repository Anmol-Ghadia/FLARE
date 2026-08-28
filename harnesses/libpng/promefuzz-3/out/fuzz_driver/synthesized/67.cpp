// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free_default at pngmem.c:245:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free_default at pngmem.c:245:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_longjmp at pngerror.c:757:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <vector>
#include <fstream>

struct FuzzMemCtx {
  size_t alloc_count;
  size_t free_count;
  size_t fail_after;
  bool force_null_alloc;
};

static void PNGAPI FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  if (png_ptr) {
    png_longjmp(png_ptr, 1);
  }
}

static void PNGAPI FuzzWarnFn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI FuzzMallocFn(png_structp, png_alloc_size_t size) {
  if (size == 0) {
    size = 1;
  }
  return malloc((size_t)size);
}

static void PNGAPI FuzzFreeFn(png_structp, png_voidp ptr) {
  free(ptr);
}

static png_voidp PNGAPI StatefulMallocFn(png_structp png_ptr, png_alloc_size_t size) {
  FuzzMemCtx *ctx = nullptr;
  if (png_ptr) {
    ctx = static_cast<FuzzMemCtx*>(png_get_mem_ptr(png_ptr));
  }

  if (ctx) {
    ctx->alloc_count++;
    if (ctx->force_null_alloc) {
      return nullptr;
    }
    if (ctx->fail_after != 0 && ctx->alloc_count >= ctx->fail_after) {
      return nullptr;
    }
  }

  if (size == 0) {
    size = 1;
  }
  return malloc((size_t)size);
}

static void PNGAPI StatefulFreeFn(png_structp png_ptr, png_voidp ptr) {
  FuzzMemCtx *ctx = nullptr;
  if (png_ptr) {
    ctx = static_cast<FuzzMemCtx*>(png_get_mem_ptr(png_ptr));
  }
  if (ctx) {
    ctx->free_count++;
  }
  free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput_67(const uint8_t *Data, size_t Size) {
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs.good() && Data && Size > 0) {
      ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  if (Data == nullptr) {
    return 0;
  }

  uint8_t b0 = Size > 0 ? Data[0] : 0;
  uint8_t b1 = Size > 1 ? Data[1] : 0;
  uint8_t b2 = Size > 2 ? Data[2] : 0;
  uint8_t b3 = Size > 3 ? Data[3] : 0;

  FuzzMemCtx ctx;
  ctx.alloc_count = 0;
  ctx.free_count = 0;
  ctx.fail_after = (b0 % 8 == 0) ? 1 : ((size_t)(b0 % 16));
  ctx.force_null_alloc = ((b1 & 1) != 0);

  png_malloc_ptr create_malloc = (b2 & 1) ? StatefulMallocFn : FuzzMallocFn;
  png_free_ptr create_free = (b2 & 2) ? StatefulFreeFn : FuzzFreeFn;
  png_voidp create_mem_ptr = (b2 & 4) ? static_cast<png_voidp>(&ctx) : nullptr;

  png_structp png_ptr = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING,
      nullptr,
      FuzzErrorFn,
      FuzzWarnFn,
      create_mem_ptr,
      create_malloc,
      create_free);

  if (!png_ptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) != 0) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_get_mem_ptr(nullptr);
  png_malloc_warn(nullptr, 16);
  png_free(nullptr, nullptr);
  png_free_default(nullptr, nullptr);
  png_set_mem_fn(nullptr, nullptr, StatefulMallocFn, StatefulFreeFn);

  png_voidp mem_before = png_get_mem_ptr(png_ptr);
  (void)mem_before;

  if (b3 & 1) {
    png_set_mem_fn(png_ptr, &ctx, StatefulMallocFn, StatefulFreeFn);
  } else if (b3 & 2) {
    png_set_mem_fn(png_ptr, nullptr, FuzzMallocFn, FuzzFreeFn);
  }

  png_voidp mem_after = png_get_mem_ptr(png_ptr);
  (void)mem_after;

  std::vector<png_voidp> ptrs;
  size_t offset = 4;
  size_t iterations = Size > 4 ? ((Size - 4) > 32 ? 32 : (Size - 4)) : 4;

  for (size_t i = 0; i < iterations; ++i) {
    png_alloc_size_t alloc_size = 0;
    if (offset + 1 < Size) {
      alloc_size = (png_alloc_size_t)((Data[offset] << 8) | Data[offset + 1]);
      offset += 2;
    } else if (offset < Size) {
      alloc_size = (png_alloc_size_t)Data[offset++];
    } else {
      alloc_size = (png_alloc_size_t)(i * 7);
    }

    switch ((i < Size ? Data[i] : (uint8_t)i) % 6) {
      case 0: {
        png_voidp p = png_malloc_warn(png_ptr, alloc_size);
        if (p) {
          memset(p, (int)(alloc_size & 0xFF), alloc_size > 256 ? 256 : (size_t)alloc_size);
          ptrs.push_back(p);
        }
        break;
      }
      case 1: {
        png_voidp p = png_malloc_warn(png_ptr, 0);
        if (p) {
          ptrs.push_back(p);
        }
        break;
      }
      case 2: {
        if (!ptrs.empty()) {
          png_free(png_ptr, ptrs.back());
          ptrs.pop_back();
        } else {
          png_free(png_ptr, nullptr);
        }
        break;
      }
      case 3: {
        void *raw = malloc((size_t)(alloc_size == 0 ? 1 : alloc_size));
        png_free_default(png_ptr, raw);
        break;
      }
      case 4: {
        png_voidp current = png_get_mem_ptr(png_ptr);
        (void)current;
        break;
      }
      case 5: {
        if (Data[i] & 1) {
          png_set_mem_fn(png_ptr, &ctx, StatefulMallocFn, StatefulFreeFn);
        } else {
          png_set_mem_fn(png_ptr, nullptr, FuzzMallocFn, FuzzFreeFn);
        }
        break;
      }
    }
  }

  while (!ptrs.empty()) {
    png_free(png_ptr, ptrs.back());
    ptrs.pop_back();
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}