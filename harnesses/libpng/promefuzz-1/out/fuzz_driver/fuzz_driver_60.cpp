// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <new>
#include <png.h>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &offset) {
  if (offset + 4 > size) return 0;
  uint32_t v = static_cast<uint32_t>(data[offset]) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               (static_cast<uint32_t>(data[offset + 2]) << 16) |
               (static_cast<uint32_t>(data[offset + 3]) << 24);
  offset += 4;
  return v;
}

static size_t ReadSizeT(const uint8_t *data, size_t size, size_t &offset) {
  return static_cast<size_t>(ReadU32(data, size, offset));
}

static void FreeIfNotNull(png_structp png_ptr, png_voidp p) {
  if (png_ptr != nullptr && p != nullptr) {
    png_free(png_ptr, p);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  // Exercise NULL handling paths.
  (void)png_get_chunk_malloc_max(nullptr);
  (void)png_get_compression_buffer_size(nullptr);
  (void)png_malloc(nullptr, 16);
  (void)png_malloc_warn(nullptr, 16);
  png_set_chunk_malloc_max(nullptr, 123);
  png_set_compression_buffer_size(nullptr, 1024);

  if (read_ptr == nullptr || write_ptr == nullptr) {
    if (read_ptr) png_destroy_read_struct(&read_ptr, nullptr, nullptr);
    if (write_ptr) png_destroy_write_struct(&write_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(read_ptr)) != 0) {
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
    png_destroy_write_struct(&write_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(write_ptr)) != 0) {
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
    png_destroy_write_struct(&write_ptr, nullptr);
    return 0;
  }

  size_t offset = 0;

  // Configure chunk malloc limits with diverse values.
  png_alloc_size_t chunk_max_r = static_cast<png_alloc_size_t>(ReadSizeT(Data, Size, offset));
  png_alloc_size_t chunk_max_w = static_cast<png_alloc_size_t>(ReadSizeT(Data, Size, offset));
  png_set_chunk_malloc_max(read_ptr, chunk_max_r);
  png_set_chunk_malloc_max(write_ptr, chunk_max_w);
  (void)png_get_chunk_malloc_max(read_ptr);
  (void)png_get_chunk_malloc_max(write_ptr);

  // Configure compression buffer sizes with diverse values, including edge cases.
  size_t comp_size_r = ReadSizeT(Data, Size, offset);
  size_t comp_size_w = ReadSizeT(Data, Size, offset);
  png_set_compression_buffer_size(read_ptr, comp_size_r);
  png_set_compression_buffer_size(write_ptr, comp_size_w);
  (void)png_get_compression_buffer_size(read_ptr);
  (void)png_get_compression_buffer_size(write_ptr);

  // Additional state exploration with derived sizes.
  size_t derived1 = Size;
  size_t derived2 = (Size > 0) ? static_cast<size_t>(Data[0]) : 0;
  size_t derived3 = 1;
  size_t derived4 = 8192;
  png_set_compression_buffer_size(read_ptr, derived1);
  png_set_compression_buffer_size(write_ptr, derived2);
  png_set_compression_buffer_size(read_ptr, derived3);
  png_set_compression_buffer_size(write_ptr, derived4);
  (void)png_get_compression_buffer_size(read_ptr);
  (void)png_get_compression_buffer_size(write_ptr);

  std::vector<png_voidp> allocations;

  // Exercise png_malloc and png_malloc_warn with multiple sizes.
  for (int i = 0; i < 8; ++i) {
    size_t req = ReadSizeT(Data, Size, offset);
    req &= 0xFFFFF;  // keep allocations bounded for fuzzing stability

    png_voidp p1 = png_malloc_warn(read_ptr, static_cast<png_alloc_size_t>(req));
    if (p1 != nullptr) {
      allocations.push_back(p1);
      if (req > 0) {
        std::memset(p1, 0xA5, req);
      }
    }

    // Keep png_malloc requests small enough to avoid OOM aborts.
    size_t safe_req = req & 0x3FFF;
    png_voidp p2 = png_malloc(write_ptr, static_cast<png_alloc_size_t>(safe_req));
    if (p2 != nullptr) {
      allocations.push_back(p2);
      if (safe_req > 0) {
        std::memset(p2, 0x5A, safe_req);
      }
    }
  }

  // Deterministic extra calls to cover more states.
  png_voidp p_small_r = png_malloc_warn(read_ptr, 1);
  if (p_small_r) {
    allocations.push_back(p_small_r);
    std::memset(p_small_r, 0, 1);
  }

  png_voidp p_small_w = png_malloc(write_ptr, 32);
  if (p_small_w) {
    allocations.push_back(p_small_w);
    std::memset(p_small_w, 0xCC, 32);
  }

  (void)png_get_chunk_malloc_max(read_ptr);
  (void)png_get_chunk_malloc_max(write_ptr);
  (void)png_get_compression_buffer_size(read_ptr);
  (void)png_get_compression_buffer_size(write_ptr);

  for (png_voidp p : allocations) {
    FreeIfNotNull(read_ptr, p);
  }

  png_destroy_read_struct(&read_ptr, nullptr, nullptr);
  png_destroy_write_struct(&write_ptr, nullptr);
  return 0;
}