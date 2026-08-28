// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <new>

static void PNGAPI fuzz_error_fn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PNGAPI fuzz_warning_fn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI custom_malloc_fn(png_structp, png_alloc_size_t size) {
  return std::malloc(size);
}

static void PNGAPI custom_free_fn(png_structp, png_voidp ptr) {
  std::free(ptr);
}

static uint64_t ReadU64(const uint8_t *data, size_t size, size_t offset) {
  uint64_t v = 0;
  for (size_t i = 0; i < 8; ++i) {
    v <<= 8;
    if (offset + i < size) v |= data[offset + i];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_81(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, fuzz_error_fn, fuzz_warning_fn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, nullptr, custom_malloc_fn, custom_free_fn);
  }

  const size_t kMaxAlloc = 1 << 20;

  png_alloc_size_t sizes[6];
  sizes[0] = static_cast<png_alloc_size_t>(Size % (kMaxAlloc + 1));
  sizes[1] = static_cast<png_alloc_size_t>((Size > 0 ? Data[0] : 0) % 256);
  sizes[2] = static_cast<png_alloc_size_t>(ReadU64(Data, Size, 0) % (kMaxAlloc + 1));
  sizes[3] = static_cast<png_alloc_size_t>(ReadU64(Data, Size, 8) % (kMaxAlloc + 1));
  sizes[4] = static_cast<png_alloc_size_t>(ReadU64(Data, Size, 16) % (kMaxAlloc + 1));
  sizes[5] = static_cast<png_alloc_size_t>((Size > 1 ? (static_cast<size_t>(Data[1]) << 8) : 0) %
                                           (kMaxAlloc + 1));

  for (int i = 0; i < 6; ++i) {
    png_voidp p1 = png_malloc_default(png_ptr, sizes[i]);
    png_free(png_ptr, p1);

    png_voidp p2 = png_malloc(png_ptr, sizes[(i + 1) % 6]);
    if (p2 != nullptr && sizes[(i + 1) % 6] > 0) {
      std::memset(p2, static_cast<int>(i), sizes[(i + 1) % 6]);
    }
    png_free(png_ptr, p2);

    png_voidp p3 = png_calloc(png_ptr, sizes[(i + 2) % 6]);
    png_free(png_ptr, p3);

    if ((Size > static_cast<size_t>(i)) && (Data[i] & 1)) {
      png_set_mem_fn(png_ptr, nullptr, custom_malloc_fn, custom_free_fn);
    }
  }

  png_free(png_ptr, nullptr);
  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}