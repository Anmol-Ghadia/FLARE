// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_free at pngmem.c:232:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
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
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>

#include <png.h>

static png_voidp CustomMalloc(png_structp png_ptr, png_alloc_size_t size) {
  (void)png_ptr;
  if (size == 0) return nullptr;
  return std::malloc(size);
}

static void CustomFree(png_structp png_ptr, png_voidp ptr) {
  (void)png_ptr;
  std::free(ptr);
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs) {
      ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);

  if (read_ptr != nullptr) {
    if (setjmp(png_jmpbuf(read_ptr)) == 0) {
      if (Size > 0 && (Data[0] & 1)) {
        png_set_mem_fn(read_ptr, nullptr, CustomMalloc, CustomFree);
      }

      size_t buf_size = 1;
      if (Size >= 4) {
        uint32_t v;
        std::memcpy(&v, Data, sizeof(v));
        buf_size = static_cast<size_t>(v);
      }
      if (buf_size == 0) buf_size = 1;
      if (buf_size > 0x7fffffffU) buf_size = 0x7fffffffU;

      png_set_compression_buffer_size(read_ptr, buf_size);

      png_alloc_size_t chunk_max = 0;
      if (Size >= 12) {
        uint64_t v = 0;
        std::memcpy(&v, Data + 4, Size >= 12 ? 8 : 0);
        chunk_max = static_cast<png_alloc_size_t>(v);
      } else if (Size >= 8) {
        uint32_t v;
        std::memcpy(&v, Data + 4, sizeof(v));
        chunk_max = static_cast<png_alloc_size_t>(v);
      }
      png_set_chunk_malloc_max(read_ptr, chunk_max);

      (void)png_get_compression_buffer_size(read_ptr);
      (void)png_get_chunk_malloc_max(read_ptr);

      size_t alloc_size = 0;
      if (Size >= 16) {
        uint32_t v;
        std::memcpy(&v, Data + 12, sizeof(v));
        alloc_size = static_cast<size_t>(v);
      } else if (Size > 0) {
        alloc_size = static_cast<size_t>(Data[Size - 1]);
      }

      size_t current_chunk_max = static_cast<size_t>(png_get_chunk_malloc_max(read_ptr));
      if (current_chunk_max != 0 && alloc_size > current_chunk_max) {
        alloc_size = current_chunk_max;
      }
      if (alloc_size > (1U << 20)) {
        alloc_size = (1U << 20);
      }

      png_voidp p1 = png_malloc(read_ptr, static_cast<png_alloc_size_t>(alloc_size));
      if (p1 != nullptr) {
        std::memset(p1, 0xA5, alloc_size);
        png_free(read_ptr, p1);
      }

      size_t alt_alloc = png_get_compression_buffer_size(read_ptr);
      if (current_chunk_max != 0 && alt_alloc > current_chunk_max) {
        alt_alloc = current_chunk_max;
      }
      if (alt_alloc > (1U << 20)) {
        alt_alloc = (1U << 20);
      }
      if (alt_alloc > 0) {
        png_voidp p2 = png_malloc(read_ptr, static_cast<png_alloc_size_t>(alt_alloc));
        if (p2 != nullptr) {
          std::memset(p2, 0x5A, alt_alloc);
          png_free(read_ptr, p2);
        }
      }
    }
  }

  if (write_ptr != nullptr) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      if (Size > 1 && (Data[1] & 1)) {
        png_set_mem_fn(write_ptr, reinterpret_cast<png_voidp>(write_ptr), CustomMalloc, CustomFree);
      }

      size_t buf_size = 1;
      if (Size >= 20) {
        uint32_t v;
        std::memcpy(&v, Data + 16, sizeof(v));
        buf_size = static_cast<size_t>(v);
      } else if (Size >= 2) {
        buf_size = static_cast<size_t>(Data[1]) + 1;
      }
      if (buf_size == 0) buf_size = 1;
      if (buf_size > 0x7fffffffU) buf_size = 0x7fffffffU;

      png_set_compression_buffer_size(write_ptr, buf_size);

      png_alloc_size_t chunk_max = 0;
      if (Size >= 28) {
        uint64_t v;
        std::memcpy(&v, Data + 20, sizeof(v));
        chunk_max = static_cast<png_alloc_size_t>(v);
      } else if (Size >= 24) {
        uint32_t v;
        std::memcpy(&v, Data + 20, sizeof(v));
        chunk_max = static_cast<png_alloc_size_t>(v);
      }
      png_set_chunk_malloc_max(write_ptr, chunk_max);

      (void)png_get_compression_buffer_size(write_ptr);
      (void)png_get_chunk_malloc_max(write_ptr);

      size_t alloc_size = png_get_compression_buffer_size(write_ptr);
      size_t current_chunk_max = static_cast<size_t>(png_get_chunk_malloc_max(write_ptr));
      if (current_chunk_max != 0 && alloc_size > current_chunk_max) {
        alloc_size = current_chunk_max;
      }
      if (alloc_size > (1U << 20)) {
        alloc_size = (1U << 20);
      }

      if (alloc_size > 0) {
        png_voidp p3 = png_malloc(write_ptr, static_cast<png_alloc_size_t>(alloc_size));
        if (p3 != nullptr) {
          std::memset(p3, 0x3C, alloc_size);
          png_free(write_ptr, p3);
        }
      }

      if (Size >= 32) {
        uint32_t v;
        std::memcpy(&v, Data + 28, sizeof(v));
        size_t second_size = static_cast<size_t>(v);
        if (current_chunk_max != 0 && second_size > current_chunk_max) {
          second_size = current_chunk_max;
        }
        if (second_size > (1U << 20)) {
          second_size = (1U << 20);
        }
        png_voidp p4 = png_malloc(write_ptr, static_cast<png_alloc_size_t>(second_size));
        if (p4 != nullptr) {
          std::memset(p4, 0xC3, second_size);
          png_free(write_ptr, p4);
        }
      }
    }
  }

  if (read_ptr != nullptr) {
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
  }
  if (write_ptr != nullptr) {
    png_destroy_write_struct(&write_ptr, nullptr);
  }

  return 0;
}