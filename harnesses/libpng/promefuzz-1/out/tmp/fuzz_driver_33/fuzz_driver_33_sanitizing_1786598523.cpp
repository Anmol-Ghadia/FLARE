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
#include <string.h>
#include <setjmp.h>
#include <vector>
#include <algorithm>

static uint64_t ReadU64(const uint8_t *data, size_t size, size_t &offset) {
  uint64_t v = 0;
  for (int i = 0; i < 8; ++i) {
    v <<= 8;
    if (offset < size) v |= data[offset++];
  }
  return v;
}

static png_structp CreatePngStruct() {
  return png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp png_ptr = CreatePngStruct();
  if (png_ptr == nullptr) return 0;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  size_t offset = 0;

  png_alloc_size_t initial_limit =
      static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
  png_set_chunk_malloc_max(png_ptr, initial_limit);
  (void)png_get_chunk_malloc_max(png_ptr);

  std::vector<png_voidp> allocated;

  size_t iterations = std::min<size_t>(32, Size > offset ? Size - offset : 0);
  for (size_t i = 0; i < iterations; ++i) {
    uint8_t op = Data[offset++];

    switch (op % 8) {
      case 0: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_malloc_warn(png_ptr, sz);
        if (p != nullptr) allocated.push_back(p);
        break;
      }
      case 1: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_calloc(png_ptr, sz);
        if (p != nullptr) allocated.push_back(p);
        break;
      }
      case 2: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_malloc(png_ptr, sz);
        if (p != nullptr) allocated.push_back(p);
        break;
      }
      case 3: {
        png_alloc_size_t new_limit =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_set_chunk_malloc_max(png_ptr, new_limit);
        break;
      }
      case 4: {
        (void)png_get_chunk_malloc_max(png_ptr);
        break;
      }
      case 5: {
        if (!allocated.empty()) {
          size_t idx = static_cast<size_t>(ReadU64(Data, Size, offset) % allocated.size());
          png_free(png_ptr, allocated[idx]);
          allocated.erase(allocated.begin() + idx);
        } else {
          png_free(png_ptr, nullptr);
        }
        break;
      }
      case 6: {
        (void)png_get_chunk_malloc_max(nullptr);
        (void)png_malloc_warn(nullptr,
                              static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset)));
        png_free(nullptr, nullptr);
        break;
      }
      case 7: {
        png_alloc_size_t cur = png_get_chunk_malloc_max(png_ptr);
        png_set_chunk_malloc_max(png_ptr, cur);
        break;
      }
    }
  }

  for (png_voidp p : allocated) {
    png_free(png_ptr, p);
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}