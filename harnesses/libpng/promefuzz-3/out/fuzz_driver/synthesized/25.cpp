// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_mem_ptr at pngmem.c:276:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <png.h>
#include <setjmp.h>

struct FuzzMemCtx {
  size_t limit;
  size_t used;
  uint8_t pattern;
};

static void PNGAPI fuzz_error_fn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PNGAPI fuzz_warn_fn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI fuzz_malloc_fn(png_structp, png_alloc_size_t size) {
  return malloc(size);
}

static void PNGAPI fuzz_free_fn(png_structp, png_voidp ptr) {
  free(ptr);
}

static png_voidp PNGAPI limited_malloc_fn(png_structp png_ptr, png_alloc_size_t size) {
  FuzzMemCtx* ctx = reinterpret_cast<FuzzMemCtx*>(png_get_mem_ptr(png_ptr));
  if (ctx == nullptr) return nullptr;
  if (size > ctx->limit) return nullptr;
  if (ctx->used > ctx->limit - size) return nullptr;
  void* p = malloc(size);
  if (p != nullptr) {
    ctx->used += size;
    memset(p, ctx->pattern, size);
  }
  return p;
}

static void PNGAPI limited_free_fn(png_structp, png_voidp ptr) {
  free(ptr);
}

static uint32_t read_u32(const uint8_t* data, size_t size, size_t& off) {
  if (off + 4 > size) {
    off = size;
    return 0;
  }
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               static_cast<uint32_t>(data[off + 3]);
  off += 4;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  size_t off = 0;
  uint32_t a = read_u32(Data, Size, off);
  uint32_t b = read_u32(Data, Size, off);
  uint32_t c = read_u32(Data, Size, off);
  uint32_t d = read_u32(Data, Size, off);
  uint32_t e = read_u32(Data, Size, off);

  png_alloc_size_t size1 = static_cast<png_alloc_size_t>(a % 65536);
  png_alloc_size_t size2 = static_cast<png_alloc_size_t>(b % 65536);
  png_alloc_size_t size3 = static_cast<png_alloc_size_t>(c % 65536);
  png_alloc_size_t size4 = static_cast<png_alloc_size_t>(d % 65536);

  FuzzMemCtx ctx;
  ctx.limit = static_cast<size_t>(1024 + (e % 131072));
  ctx.used = 0;
  ctx.pattern = static_cast<uint8_t>(e & 0xFF);

  png_structp png_default = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING, nullptr, fuzz_error_fn, fuzz_warn_fn,
      nullptr, nullptr, nullptr);

  if (png_default != nullptr) {
    png_voidp p1 = nullptr;
    png_voidp p2 = nullptr;
    png_voidp p3 = nullptr;
    png_voidp p4 = nullptr;

    if (setjmp(png_jmpbuf(png_default)) == 0) {
      p1 = png_malloc_warn(png_default, size1);
      p2 = png_calloc(png_default, size2);
      if ((a & 1U) != 0) {
        png_set_mem_fn(png_default, nullptr, fuzz_malloc_fn, fuzz_free_fn);
      }
      p3 = png_malloc_warn(png_default, size3);
      if ((b & 1U) != 0) {
        p4 = png_malloc(png_default, size4);
      }
    }

    png_free(png_default, p1);
    png_free(png_default, p2);
    png_free(png_default, p3);
    png_free(png_default, p4);
    png_free(png_default, nullptr);
    png_destroy_read_struct(&png_default, nullptr, nullptr);
  }

  png_structp png_custom = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING, &ctx, fuzz_error_fn, fuzz_warn_fn,
      &ctx, limited_malloc_fn, limited_free_fn);

  if (png_custom != nullptr) {
    png_voidp q1 = nullptr;
    png_voidp q2 = nullptr;
    png_voidp q3 = nullptr;
    png_voidp q4 = nullptr;
    png_voidp q5 = nullptr;

    if (setjmp(png_jmpbuf(png_custom)) == 0) {
      q1 = png_malloc_warn(png_custom, size1);
      q2 = png_calloc(png_custom, size2);

      if ((c & 1U) != 0) {
        png_set_mem_fn(png_custom, &ctx, limited_malloc_fn, limited_free_fn);
      } else {
        png_set_mem_fn(png_custom, nullptr, fuzz_malloc_fn, fuzz_free_fn);
      }

      q3 = png_malloc_warn(png_custom, size3);
      if ((d & 1U) != 0) {
        q4 = png_malloc(png_custom, size4);
      }
      q5 = png_calloc(png_custom, static_cast<png_alloc_size_t>((size1 + size2) % 65536));
    }

    png_free(png_custom, q1);
    png_free(png_custom, q2);
    png_free(png_custom, q3);
    png_free(png_custom, q4);
    png_free(png_custom, q5);
    png_destroy_read_struct(&png_custom, nullptr, nullptr);
  }

  (void)png_malloc_warn(nullptr, size1);
  png_free(nullptr, nullptr);

  return 0;
}