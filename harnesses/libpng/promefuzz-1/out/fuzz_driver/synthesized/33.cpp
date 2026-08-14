// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_calloc at pngmem.c:49:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_malloc_warn at pngmem.c:211:1 in png.h
// png_free at pngmem.c:232:1 in png.h
#include <cstdint>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <setjmp.h>
#include <png.h>

static uint64_t ReadU64(const uint8_t *data, size_t size, size_t &offset) {
  uint64_t v = 0;
  for (int i = 0; i < 8; ++i) {
    v <<= 8;
    if (offset < size) {
      v |= data[offset++];
    }
  }
  return v;
}

static png_structp CreatePngStruct() {
  return png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput_33(const uint8_t *Data, size_t Size) {
  png_structp png_ptr = CreatePngStruct();
  if (png_ptr == nullptr) {
    return 0;
  }

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

  size_t max_iterations = std::min<size_t>(32, Size > offset ? Size - offset : 0);
  for (size_t i = 0; i < max_iterations && offset < Size; ++i) {
    uint8_t op = Data[offset++];

    switch (op % 8) {
      case 0: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_malloc_warn(png_ptr, sz);
        if (p != nullptr) {
          allocated.push_back(p);
        }
        break;
      }
      case 1: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_calloc(png_ptr, sz);
        if (p != nullptr) {
          allocated.push_back(p);
        }
        break;
      }
      case 2: {
        png_alloc_size_t sz =
            static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset));
        png_voidp p = png_malloc(png_ptr, sz);
        if (p != nullptr) {
          allocated.push_back(p);
        }
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
          size_t idx =
              static_cast<size_t>(ReadU64(Data, Size, offset) % allocated.size());
          png_free(png_ptr, allocated[idx]);
          allocated.erase(allocated.begin() + idx);
        } else {
          png_free(png_ptr, nullptr);
        }
        break;
      }
      case 6: {
        (void)png_get_chunk_malloc_max(nullptr);
        (void)png_malloc_warn(
            nullptr, static_cast<png_alloc_size_t>(ReadU64(Data, Size, offset)));
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