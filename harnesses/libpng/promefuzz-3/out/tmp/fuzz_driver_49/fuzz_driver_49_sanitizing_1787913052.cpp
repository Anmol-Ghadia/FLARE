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

static void FuzzPngError(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzPngWarn(png_structp, png_const_charp) {
}

static png_voidp FuzzMalloc(png_structp, png_alloc_size_t size) {
  if (size == 0) return NULL;
  return malloc(size);
}

static void FuzzFree(png_structp, png_voidp ptr) {
  free(ptr);
}

static uint32_t ReadU32(const uint8_t *data) {
  return (static_cast<uint32_t>(data[0]) << 24) |
         (static_cast<uint32_t>(data[1]) << 16) |
         (static_cast<uint32_t>(data[2]) << 8) |
         static_cast<uint32_t>(data[3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, FuzzPngError, FuzzPngWarn);
  if (png_ptr == NULL) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, FuzzMalloc, FuzzFree);
  }

  size_t offset = 1;

  if (Size - offset >= 4) {
    png_alloc_size_t chunk_max = static_cast<png_alloc_size_t>(ReadU32(Data + offset));
    png_set_chunk_malloc_max(png_ptr, chunk_max);
    offset += 4;
  } else {
    png_set_chunk_malloc_max(png_ptr, 0);
  }

  if (Size - offset >= 4) {
    uint32_t raw = ReadU32(Data + offset);
    size_t buf_size = static_cast<size_t>(raw);
    png_set_compression_buffer_size(png_ptr, buf_size);
    offset += 4;
  } else {
    png_set_compression_buffer_size(png_ptr, 1);
  }

  png_set_packing(png_ptr);

  for (int i = 0; i < 4; ++i) {
    png_alloc_size_t alloc_size = 0;
    if (Size - offset >= 4) {
      alloc_size = static_cast<png_alloc_size_t>(ReadU32(Data + offset));
      offset += 4;
    } else if (Size > 0) {
      alloc_size = static_cast<png_alloc_size_t>(Data[(offset + i) % Size]);
    }

    if (alloc_size > 0) {
      png_voidp p = png_malloc(png_ptr, alloc_size);
      if (p != NULL) {
        memset(p, 0xA5, static_cast<size_t>(alloc_size));
        png_free(png_ptr, p);
      }
    } else {
      png_voidp p = png_malloc(png_ptr, 0);
      if (p != NULL) {
        png_free(png_ptr, p);
      }
    }

    if (Size > 0) {
      if (Data[i % Size] & 1) {
        png_set_packing(png_ptr);
      }
      if (Data[i % Size] & 2) {
        png_set_chunk_malloc_max(
            png_ptr, static_cast<png_alloc_size_t>(Data[i % Size]) * 1024u);
      }
      if (Data[i % Size] & 4) {
        size_t s = static_cast<size_t>(Data[i % Size]);
        if (s == 0) s = 1;
        png_set_compression_buffer_size(png_ptr, s);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}