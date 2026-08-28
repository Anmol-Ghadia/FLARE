// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <png.h>
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

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset, uint32_t fallback) {
  if (data == NULL || offset > size || size - offset < 4) return fallback;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

extern "C" int LLVMFuzzerTestOneInput_49(const uint8_t *Data, size_t Size) {
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

  if (Size > 0 && Data != NULL && (Data[0] & 1)) {
    png_set_mem_fn(png_ptr, NULL, FuzzMalloc, FuzzFree);
  }

  size_t offset = 1;

  png_alloc_size_t chunk_max =
      static_cast<png_alloc_size_t>(ReadU32(Data, Size, offset, 0));
  png_set_chunk_malloc_max(png_ptr, chunk_max);
  if (offset <= Size && Size - offset >= 4) offset += 4;

  size_t buf_size = static_cast<size_t>(ReadU32(Data, Size, offset, 1));
  if (buf_size == 0) buf_size = 1;
  png_set_compression_buffer_size(png_ptr, buf_size);
  if (offset <= Size && Size - offset >= 4) offset += 4;

  png_set_packing(png_ptr);

  for (int i = 0; i < 4; ++i) {
    png_alloc_size_t alloc_size = 0;
    if (Data != NULL && offset <= Size && Size - offset >= 4) {
      alloc_size = static_cast<png_alloc_size_t>(ReadU32(Data, Size, offset, 0));
      offset += 4;
    } else if (Data != NULL && Size > 0) {
      alloc_size = static_cast<png_alloc_size_t>(Data[(offset + static_cast<size_t>(i)) % Size]);
    }

    png_voidp p = png_malloc(png_ptr, alloc_size);
    if (p != NULL) {
      if (alloc_size > 0) {
        memset(p, 0xA5, static_cast<size_t>(alloc_size));
      }
      png_free(png_ptr, p);
    }

    if (Data != NULL && Size > 0) {
      uint8_t b = Data[static_cast<size_t>(i) % Size];
      if (b & 1) {
        png_set_packing(png_ptr);
      }
      if (b & 2) {
        png_set_chunk_malloc_max(
            png_ptr, static_cast<png_alloc_size_t>(b) * 1024u);
      }
      if (b & 4) {
        size_t s = static_cast<size_t>(b);
        if (s == 0) s = 1;
        png_set_compression_buffer_size(png_ptr, s);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}